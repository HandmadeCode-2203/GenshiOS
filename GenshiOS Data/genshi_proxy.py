import serial
import requests
import textwrap
import threading
import sys
import time
from bs4 import BeautifulSoup

PORT = '/dev/cu.usbserial-A5069RR4'
BAUD = 2000000
SCREEN_WIDTH = 45
def gui_to_tui_converter(url):
    """
    Webサイト(GUI)を解析して、綺麗なTUIテキストに自動変換するコアエンジン
    """
    try:
        if not url.startswith('http'):
            url = 'https://' + url
        headers = {'User-Agent': 'Mozilla/5.0 (GenshiOS TUI Browser 2026)'}
        response = requests.get(url, headers=headers, timeout=5)
        response.encoding = response.apparent_encoding
        
        soup = BeautifulSoup(response.text, 'html.parser')
        for script in soup(["script", "style", "nav", "footer", "iframe", "head", "form"]):
            script.decompose()
        tui_lines = []
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        tui_lines.append(f"|  GenshiOS TUI BROWSER v1.0  |")
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        title = soup.title.string.strip() if soup.title else "No Title"
        wrapped_title = textwrap.wrap(f"SITE: {title}", SCREEN_WIDTH - 4)
        for line in wrapped_title:
            tui_lines.append(f"| {line.ljust(SCREEN_WIDTH-4)} |")
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        content_count = 0
        for element in soup.find_all(['h1', 'h2', 'h3', 'p']):
            text = element.get_text().strip()
            if not text or len(text) < 0:
                continue
            if element.name.startswith('h'):
                text = f"■ {text}"
            wrapped_text = textwrap.wrap(text, SCREEN_WIDTH - 4)
            for line in wrapped_text:
                tui_lines.append(f"| {line.ljust(SCREEN_WIDTH-4)} |")
            tui_lines.append(f"| {' '*(SCREEN_WIDTH-4)} |")
            content_count += 1
            if content_count > 15:
                break
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        tui_lines.append(f"| [End of Page]                      |")
        tui_lines.append("+" + "-"*(SCREEN_WIDTH-2) + "+")
        
        return tui_lines

    except Exception as e:
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
                raw_data = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
                buffer += raw_data
                if "__NET_REQ:" in buffer and "__" in buffer.split("__NET_REQ:")[1]:
                    parts = buffer.split("__NET_REQ:")
                    sub_parts = parts[1].split("__")
                    url = sub_parts[0].strip()
                    sys.stdout.write(parts[0])
                    sys.stdout.flush()
                    buffer = ""
                    print(f"\n[Proxy] Fetching and Converting: {url}...")
                    tui_data = gui_to_tui_converter(url)
                    
                    print(f"[Proxy] Injection started via 2Mbps Link...")
                    for line in tui_data:
                        ser.write((line + "\n").encode('utf-8'))
                        time.sleep(0.005)
                    time.sleep(0.005)
                    ser.write("__NET_END__\n".encode('utf-8'))
                    print(f"[Proxy] Injection completed successfully.\n")
                else:
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
    rx_thread = threading.Thread(target=serial_receive_thread, args=(ser,), daemon=True)
    rx_thread.start()
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