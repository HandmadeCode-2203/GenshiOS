import serial
import requests
import textwrap
import threading
import sys
import time
from bs4 import BeautifulSoup

# ==========================================
#                  ⚙️ 設定
# ==========================================
PORT = '/dev/cu.usbserial-A5069RR4'  # Macが認識しているGenshiOSのポート
BAUD = 2000000                        # 2Mbpsの超高速シンク
SCREEN_WIDTH = 45                     # GenshiOSの画面に合わせた横幅

def gui_to_tui_converter(url):
    """
    Webサイト(GUI)を解析して、綺麗なTUIテキストに自動変換するコアエンジン
    """
    try:
        # URLの自動補完
        if not url.startswith('http'):
            url = 'https://' + url
            
        # 拒絶されないように一般的なブラウザのUser-Agentを偽装
        headers = {'User-Agent': 'Mozilla/5.0 (GenshiOS TUI Browser 2026)'}
        response = requests.get(url, headers=headers, timeout=5)
        response.encoding = response.apparent_encoding # 文字化け対策
        
        soup = BeautifulSoup(response.text, 'html.parser')
        
        # 画面レイアウトを破壊する不要な非テキストタグを完全に消し去る
        for script in soup(["script", "style", "nav", "footer", "iframe", "head", "form"]):
            script.decompose()

        tui_lines = []
        
        # --- TUI ヘッダーの生成 ---
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        tui_lines.append(f"|  GenshiOS TUI BROWSER v1.0  |")
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        
        # サイトタイトルの抽出と整形
        title = soup.title.string.strip() if soup.title else "No Title"
        wrapped_title = textwrap.wrap(f"SITE: {title}", SCREEN_WIDTH - 4)
        for line in wrapped_title:
            tui_lines.append(f"| {line.ljust(SCREEN_WIDTH-4)} |")
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")

        # --- 本文・見出しの自動抽出と折り返しレンダリング ---
        content_count = 0
        for element in soup.find_all(['h1', 'h2', 'h3', 'p']):
            text = element.get_text().strip()
            if not text or len(text) < 5: # ゴミテキストのフィルタリング
                continue
                
            # 見出し（hタグ）なら装飾マークを付与
            if element.name.startswith('h'):
                text = f"■ {text}"
                
            # 2KBのRAM側で溢れないよう、指定の横幅でテキストを自動改行
            wrapped_text = textwrap.wrap(text, SCREEN_WIDTH - 4)
            for line in wrapped_text:
                tui_lines.append(f"| {line.ljust(SCREEN_WIDTH-4)} |")
            tui_lines.append(f"| {' '*(SCREEN_WIDTH-4)} |") # 段落間の空行
            
            content_count += 1
            if content_count > 15: # Arduinoの処理限界を考慮して適度な行数で打ち切り
                break
                
        # --- TUI フッターの生成 ---
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        tui_lines.append(f"| [End of Page]                      |")
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        
        return tui_lines

    except Exception as e:
        # エラー発生時はエラー内容をTUI枠に入れて返す
        error_msg = f"Error: {str(e)}"
        wrapped_err = textwrap.wrap(error_msg, SCREEN_WIDTH - 4)
        err_lines = ["+" + "-"*(SCREEN_WIDTH-2) + "+"]
        for line in wrapped_err:
            err_lines.append(f"| {line.ljust(SCREEN_WIDTH-4)} |")
        err_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        return err_lines

def serial_receive_thread(ser):
    """
    GenshiOS(Arduino)からのデータ送信をバックグラウンドで常時監視するスレッド
    """
    buffer = ""
    while True:
        try:
            if ser.in_waiting:
                # シリアルポートから届いたデータを読み込む
                raw_data = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
                buffer += raw_data
                
                # ─── ネットアクセス要求「__NET_REQ:URL__」を検知した場合 ───
                if "__NET_REQ:" in buffer and "__" in buffer.split("__NET_REQ:")[1]:
                    parts = buffer.split("__NET_REQ:")
                    sub_parts = parts[1].split("__")
                    url = sub_parts[0].strip()
                    
                    # 特殊コマンドの手前までの文字列（wgetコマンドの残骸など）を標準出力
                    sys.stdout.write(parts[0])
                    sys.stdout.flush()
                    buffer = ""
                    
                    print(f"\n[Proxy] Fetching and Converting: {url}...")
                    
                    # インターネットからデータを取得してTUIに自動変換
                    tui_data = gui_to_tui_converter(url)
                    
                    print(f"[Proxy] Injection started via 2Mbps Link...")
                    
                    # 💡 バッファ溢れ（流速オーバー）対策を施したデータ射出
                    for line in tui_data:
                        ser.write((line + "\n").encode('utf-8'))
                        time.sleep(0.005)  # 5ミリ秒の優しさ（Arduinoが受信を処理する猶予）
                    
                    # Arduinoにデータ送信が完了したことを伝える終了シグナル
                    time.sleep(0.005)
                    ser.write("__NET_END__\n".encode('utf-8'))
                    print(f"[Proxy] Injection completed successfully.\n")
                
                # ─── 通常のOS出力（プロンプトや他のコマンドの結果）の場合 ───
                else:
                    # 特殊キーワードの解析途中でなければ、そのままリアルタイムに画面に流す
                    if "__NET_REQ:" not in buffer:
                        sys.stdout.write(buffer)
                        sys.stdout.flush()
                        buffer = ""
        except Exception:
            print("\n[Proxy] Disconnected from serial thread.")
            break

def main():
    print(f"[Proxy] Connecting to GenshiOS on {PORT} at {BAUD} baud...")
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except Exception as e:
        print(f"[Error] Cannot open port: {e}")
        print("[Hint] Make sure Arduino IDE Serial Monitor is CLOSED.")
        return

    print("[Proxy] Connected! GenshiOS Terminal Mode active.")
    print("[Hint] Press 'Enter' once if the prompt '>' doesn't appear.\n")

    # Arduinoからのデータ受信スレッドを裏で起動
    rx_thread = threading.Thread(target=serial_receive_thread, args=(ser,), daemon=True)
    rx_thread.start()

    # パソコンのキーボード入力を常時監視し、Arduino(GenshiOS)へ送り続けるメインループ
    while True:
        try:
            user_input = input()
            ser.write((user_input + "\n").encode('utf-8'))
        except (KeyboardInterrupt, EOFError):
            print("\n[Proxy] Closing connection...")
            ser.close()
            break

if __name__ == '__main__':
    main()