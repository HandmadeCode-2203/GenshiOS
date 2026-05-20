#include <avr/pgmspace.h>
const char frame_data[] PROGMEM = 
  "FRAME:[GenshiOS_GPU_TEST_PATTERN_V1]-------------------\n"
  "| * . * . * . * . * . * . * . * . * . * . * . * . * . |\n"
  "| .   G e n s h i O S   R e n d e r   E n g i n e   . |\n"
  "| * . * . * . * . * . * . * . * . * . * . * . * . * . |\n"
  "------------------------------------------------------\n";

String command = "";
String memo_data = "";
String file_names[3] = {"", "", ""};
String file_contents[3] = {"", "", ""};
int file_count = 0;
long ans = 0;
String content = ("");
unsigned long previousMillis = 0;
const long interval = 100;
int ledState = LOW;
const int ledPin = LED_BUILTIN;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(2000000);
  
  Serial.println(F("========================="));
  Serial.println(F("---Welcome to GenshiOS---"));
  Serial.println(F("========================="));
  Serial.print(F(" > "));
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      command.trim();
      
      if (command == "help") {
        Serial.println(command);
        Serial.println(F("======================================="));
        Serial.println(F("         GenshiOS Help Menu            "));
        Serial.println(F("======================================="));
        Serial.println(F("# Available Commands #"));
        Serial.println(F("  help      - Show this help menu"));
        Serial.println(F("  cal       - Start the calculator"));
        Serial.println(F("  memo      - Open System Memo (Create scripts)"));
        Serial.println(F("  ls        - List files in system storage"));
        Serial.println(F("  cat       - Read text file contents"));
        Serial.println(F("  run [f]   - Execute a GenshiScript (.gns)"));
        Serial.println(F("  pi        - Run Monte Carlo Pi Benchmark"));
        Serial.println(F("  fps       - Run Graphical Rendering Benchmark")); 
        Serial.println(F("  all       - Run Full Suite Benchmark")); 
        Serial.println(F("  game      - Launch Realtime TUI Game Engine"));
        Serial.println(F("======================================="));
        Serial.println();
        Serial.print(F(" > "));
        command = "";
      }
      else if (command == "cal") {
        Serial.println(command);
        command = "";
        calc();
      }
      else if (command == "memo"){
        Serial.println(command);
        command = "";
        memo();
      }
      else if (command == "ls"){
        Serial.println(command);
        command = "";
        list_files();
      }
      else if (command == "cat"){
        Serial.println(command);
        command = "";
        cat_file();
      }
      else if (command == "pi"){
        Serial.println(command);
        command = "";
        run_pi_benchmark();
      }
      else if (command == "fps"){ 
        Serial.println(command);
        command = "";
        run_fps_benchmark();
      }
      else if (command == "all"){ 
        Serial.println(command);
        command = "";
        run_total_benchmark();
      }
      else if (command == "game"){ 
        Serial.println(command);
        command = "";
        game_invader();
      }
      else if (command.startsWith("run ")) {
        Serial.println(command); 
        String target_file = command.substring(4); 
        target_file.trim();
        run_executable(target_file); 
        command = "";
      }
      else if (command.startsWith("wget ")) {
        Serial.println(command); 
        String target = command.substring(5); 
        target.trim();
        Serial.print(F("__NET_REQ:"));
        Serial.print(target);
        Serial.println(F("__"));
        Serial.println(F("[GenshiOS_NET] Connecting to proxy..."));
        unsigned long netTimeout = millis();
        while (millis() - netTimeout < 5000) {
          if (Serial.available() > 0) {
            String res = Serial.readStringUntil('\n');
            res.trim();
            if (res == "__NET_END__") {
              break;
            }
            Serial.println(res);
            netTimeout = millis();
          }
        }
        Serial.println(F("[GenshiOS_NET] Connection closed."));
        Serial.println();
        Serial.print(F(" > "));
        command = "";
      }
      else {
        if (command != "") {
          Serial.println(command);
          Serial.println();
          Serial.println(F("##command is not found##"));
          Serial.println();
          Serial.print(F(" > "));
        } else {
          Serial.print(F(" > "));
        }
        command = "";
      }
    }
    else {
      command += c;
    }
  }
}

void calc(){
  Serial.println();
  Serial.println(F("enter the value1"));
  Serial.print(F(" > "));

  long value1;
  while(true){
    if (Serial.available() > 0) {
      value1 = Serial.parseInt();
      while(Serial.available() > 0 && (Serial.peek() == '\n' || Serial.peek() == '\r')) Serial.read();
      break;
    }
  }
  Serial.println(value1);
  Serial.println();
  Serial.println(F("enter the value2"));
  Serial.print(F(" > "));
  
  long value2;
  while (true){
    if (Serial.available() > 0){
      value2 = Serial.parseInt();
      while(Serial.available() > 0 && (Serial.peek() == '\n' || Serial.peek() == '\r')) Serial.read();
      break;
    }
  }
  Serial.println(value2);
  Serial.println();
  Serial.println(F("enter the operator"));
  Serial.print(F(" > "));
  
  char op;
  while (true){
    if (Serial.available() > 0){
      op = Serial.read();
      if (op != '\n' && op != '\r' && op != ' ') {
        break;
      }
    }
  }
  Serial.println(op); 
  
  if (op == '+') ans = value1 + value2;
  else if (op == '-') ans = value1 - value2;
  else if (op == '*') ans = value1 * value2;
  else if (op == '/') {
    if(value2 == 0) Serial.println(F("Zero Division Error"));
    else ans = value1 / value2;
  }
  else {
    ans = 0;
    Serial.println(F("Error: Invalid Operator"));
  }
  
  Serial.print(F("Result: "));
  Serial.println(ans); 
  Serial.println();
  Serial.print(F(" > "));
  ans = 0;
  value1 = 0;
  value2 = 0;
  op = ';';
}

void memo(){
  Serial.println(F("======================================="));
  Serial.println(F("--- GenshiOS System Memo Ver 1.0 ---"));
  Serial.println(F("======================================="));
  Serial.println();
  Serial.print(F(" | "));
  
  while (true){
    if (Serial.available() >= 2){
        content = Serial.readStringUntil('\n');
        content.trim();
        
        if (content != "exit genshios system memo" && content != "view history") {
          memo_data += content + '\n';
          Serial.println(content);
        }
        
        if(content == "view history"){
          Serial.println(F("[History]")); 
          Serial.println(memo_data);
        }
        if (content == "exit genshios system memo"){
          Serial.println(F("[Exiting Memo App]")); 
          Serial.println();
          Serial.println(F("Do you want to save this memo as a file? (y/n)"));
          Serial.print(F(" > "));
          
          char choice = ' ';
          while(true) {
            if (Serial.available() > 0) {
              choice = Serial.read();
              if (choice == 'y' || choice == 'n') break;
            }
          }
          Serial.println(choice);
          if (choice == 'y') {
            int target_index = -1;
            for (int i = 0; i < 3; i++) {
              if (file_names[i] == "") {
                target_index = i;
                break;
              }
            }
            if (target_index == -1) {
              Serial.println(F("Error: Disk Full! (Max 3 files). Cannot save."));
            } 
            else {
              while(Serial.available() > 0) Serial.read(); 
              Serial.print(F("Enter file name > "));
              String new_name = "";
              while (true) {
                if (Serial.available() > 0) {
                  new_name = Serial.readStringUntil('\n');
                  new_name.trim();
                  if (new_name != "") break;
                }
              }
              if (!new_name.endsWith(".txt") && !new_name.endsWith(".gns")) {
                new_name += ".txt";
              }
              Serial.println(new_name);
              file_names[target_index] = new_name;
              file_contents[target_index] = memo_data;
              file_count++;
              Serial.println(F("SUCCESS: Saved to GenshiOS Files!"));
            }
          }
          memo_data = ""; 
          Serial.println();
          Serial.print(F(" > "));
          break;
        }
        if (content != "exit genshios system memo") {
          Serial.print(F(" | "));
        }
    }
  }
}

void list_files() {
  Serial.println();
  Serial.println(F("--- Files in GenshiOS ---"));
  if (file_count == 0) {
    Serial.println(F(" (No files found)"));
  }
  else {
    for (int i = 0; i < 3; i++) {
      if (file_names[i] != "") {
        Serial.print(F(" - "));
        if (file_names[i].endsWith(".gns")) Serial.print(F("[GNS] "));
        else Serial.print(F("[TXT] "));
        Serial.println(file_names[i]);
      }
    }
  }
  Serial.println(F("-------------------------"));
  Serial.println();
  Serial.print(F(" > "));
}

void cat_file() {
  while(Serial.available() > 0) Serial.read();
  Serial.println();
  Serial.print(F("Enter file name to read > "));
  String target = "";
  while (true) {
    if (Serial.available() > 0) {
      target = Serial.readStringUntil('\n');
      target.trim();
      break;
    }
  }
  Serial.println(target);
  bool found = false;
  for (int i = 0; i < 3; i++) {
    if (file_names[i] == target) {
      Serial.println();
      Serial.print(F("----- [ ")); Serial.print(file_names[i]); Serial.println(F(" ] -----"));
      Serial.print(file_contents[i]);
      Serial.println(F("-------------------------"));
      found = true;
      break;
    }
  }
  if (!found) {
    Serial.println();
    Serial.println(F("Error: File not found."));
  }
  Serial.println();
  Serial.print(F(" > "));
}

void run_executable(String target) {
  if (!target.endsWith(".gns")) {
    Serial.println();
    Serial.println(F("Error: Not a GenshiScript (.gns) file."));
    Serial.println();
    Serial.print(F(" > "));
    return;
  }
  int found_index = -1;
  for (int i = 0; i < 3; i++) {
    if (file_names[i] == target) {
      found_index = i;
      break;
    }
  }
  if (found_index == -1) {
    Serial.println();
    Serial.println(F("Error: File not found."));
    Serial.println();
    Serial.print(F(" > "));
    return;
  }
  Serial.println();
  Serial.print(F(" Executing [ ")); Serial.print(target); Serial.println(F(" ]..."));
  Serial.println(F("---------------------------------------"));

  String script = file_contents[found_index];
  while (script.length() > 0) {
    int next_line_index = script.indexOf('\n');
    String line = "";
    if (next_line_index == -1) {
      line = script;
      script = "";
    } else {
      line = script.substring(0, next_line_index);
      script = script.substring(next_line_index + 1);
    }
    line.trim();
    if (line == "") continue;
    if (line.startsWith("PRINT ")) {
      String msg = line.substring(6);
      Serial.println(msg);
    }
    else if (line == "LED HIGH") {
      digitalWrite(ledPin, HIGH);
      Serial.println(F("[OS_KERNEL] Hardware LED -> HIGH"));
    }
    else if (line == "LED LOW") {
      digitalWrite(ledPin, LOW);
      Serial.println(F("[OS_KERNEL] Hardware LED -> LOW"));
    }
    else {
      Serial.print(F("Syntax Error: Unknown instruction -> "));
      Serial.println(line);
    }
  }
  Serial.println(F("---------------------------------------"));
  Serial.println(F("SUCCESS: Process finished with exit code 0."));
  Serial.println();
  Serial.print(F(" > "));
}

int getFreeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void run_pi_benchmark() {
  while(Serial.available() > 0) Serial.read();
  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("--- GenshiOS Pi Performance Tester ---"));
  Serial.println(F("======================================="));
  Serial.print(F("Enter benchmark duration (seconds) > "));
  long duration_sec = 0;
  while (true) {
    if (Serial.available() > 0) {
      duration_sec = Serial.parseInt();
      if (duration_sec > 0) break;
    }
  }
  Serial.println(duration_sec);
  
  Serial.println();
  Serial.println(F("Preparing Benchmark Engine..."));
  delay(500);
  Serial.println(F("STARTING... (Press any key to abort)"));
  Serial.println(F("------------------------------------------------------------------------"));
  Serial.println(F(" [Dots]   [Current Pi]   [Elapsed]   [Latency/dot]   [CPU]   [Free RAM]"));
  Serial.println(F("------------------------------------------------------------------------"));

  unsigned long start_time = millis();
  unsigned long target_duration = duration_sec * 1000;
  unsigned long last_report_time = 0;
  
  unsigned long total_dots = 0;
  unsigned long inside_circle = 0;
  randomSeed(analogRead(A0));

  while (Serial.available() > 0) {
    Serial.read();
    delay(10); 
  }
  
  while (millis() - start_time < target_duration) {
    unsigned long dot_start = micros();
    
    long x = random(0, 10001);
    long y = random(0, 10001);
    
    if ((x * x) + (y * y) <= 100000000L) {
      inside_circle++;
    }
    total_dots++;
    
    unsigned long dot_end = micros();
    unsigned long latency = dot_end - dot_start;
    
    if (millis() - last_report_time >= 1000) {
      last_report_time = millis();
      float current_pi = ((float)inside_circle / (float)total_dots) * 4.0;
      float elapsed_seconds = (float)(millis() - start_time) / 1000.0;
      
      Serial.print(" ");
      Serial.print(total_dots);
      Serial.print("\t  ");
      Serial.print(current_pi, 5);
      Serial.print("\t ");
      Serial.print(elapsed_seconds, 1); Serial.print("s");
      Serial.print("\t    ");
      Serial.print(latency); Serial.print(" us");
      Serial.print(F("\t     100%\t "));
      Serial.print(getFreeRam()); Serial.println(F(" Bytes"));
    }

    if (Serial.available() > 0) {
      Serial.println(F("\n[BENCHMARK] Aborted by user."));
      while(Serial.available() > 0) Serial.read();
      break;
    }
  }
  unsigned long total_elapsed = millis() - start_time;
  float final_pi = ((float)inside_circle / (float)total_dots) * 4.0;
  float avg_latency = (float)total_elapsed / (float)total_dots * 1000.0;
  
  Serial.println(F("------------------------------------------------------------------------"));
  Serial.println(F("--- BENCHMARK REPORT ---"));
  Serial.print(F("  Total Time Elapsed : ")); Serial.print((float)total_elapsed / 1000.0, 3); Serial.println(F(" seconds"));
  Serial.print(F("  Total Dots Plotted : ")); Serial.println(total_dots);
  Serial.print(F("  Calculated Pi Value: ")); Serial.println(final_pi, 6);
  Serial.print(F("  Average Latency/Dot: ")); Serial.print(avg_latency, 2); Serial.println(F(" microseconds"));
  Serial.print(F("  Final System RAM   : ")); Serial.print(getFreeRam()); Serial.println(F(" Bytes free"));
  Serial.println(F("---------------------------------------"));
  
  Serial.println();
  Serial.print(F(" > "));
}

void run_fps_benchmark() {
  while(Serial.available() > 0) Serial.read();
  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("--- GenshiOS Display Link Tester Ver1.0 ---"));
  Serial.println(F("======================================="));
  Serial.print(F("Enter benchmark duration (seconds) > "));
  long duration_sec = 0;
  while (true) {
    if (Serial.available() > 0) {
      duration_sec = Serial.parseInt();
      if (duration_sec > 0) break;
    }
  }
  Serial.println(duration_sec);
  
  Serial.println();
  Serial.println(F("Testing Display Stream..."));
  delay(500);
  Serial.println(F("STARTING... (Press any key to abort)"));
  delay(1000);

  size_t frame_len = strlen_P(frame_data);
  unsigned long start_time = millis();
  unsigned long target_duration = duration_sec * 1000;
  unsigned long total_frames = 0;

  while (Serial.available() > 0) {
    Serial.read();
    delay(10); 
  }

  while (millis() - start_time < target_duration) {
    int p = 0;
    while (p < frame_len) {
      char chr = pgm_read_byte_near(frame_data + p);
      Serial.print(chr);
      p++;
    }
    total_frames++;

    if (Serial.available() > 0) {
      Serial.println(F("\n[BENCHMARK] Aborted by user."));
      while(Serial.available() > 0) Serial.read();
      break;
    }
  }

  unsigned long total_elapsed = millis() - start_time;
  float elapsed_sec = (float)total_elapsed / 1000.0;
  float fps = (float)total_frames / elapsed_sec;
  unsigned long total_bytes = total_frames * frame_len;
  float bytes_per_sec = (float)total_bytes / elapsed_sec;
  float bus_saturation = (bytes_per_sec / 960.0) * 100.0;

  Serial.println(F("\n---------------------------------------"));
  Serial.println(F("--- RENDERING REPORT ---"));
  Serial.print(F("  Total Time Elapsed  : ")); Serial.print(elapsed_sec, 3); Serial.println(F(" seconds"));
  Serial.print(F("  Total Frames Drawn  : ")); Serial.println(total_frames);
  Serial.print(F("  Calculated FrameRate: ")); Serial.print(fps, 2); Serial.println(F(" FPS"));
  Serial.print(F("  Data Transfer Rate  : ")); Serial.print(bytes_per_sec, 1); Serial.println(F(" Bytes/sec"));
  Serial.print(F("  Serial Bus Load     : ")); Serial.print(bus_saturation, 1); Serial.println(F(" % (At 9600 baud)"));
  Serial.println(F("---------------------------------------"));
  
  Serial.println();
  Serial.print(F(" > "));
}

void run_total_benchmark() {
  while(Serial.available() > 0) Serial.read();
  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("--- GenshiOS Total System Evaluator ---"));
  Serial.println(F("======================================="));
  Serial.print(F("Enter duration per test (seconds) > "));
  long duration_sec = 0;
  while (true) {
    if (Serial.available() > 0) {
      duration_sec = Serial.parseInt();
      if (duration_sec > 0) break;
    }
  }
  Serial.println(duration_sec);
  
  Serial.println();
  Serial.println(F("[1/2] Launching Pi Benchmark Engine..."));
  delay(1000);
  unsigned long pi_start_time = millis();
  unsigned long target_duration = duration_sec * 1000;
  unsigned long last_report_time = 0;
  unsigned long total_dots = 0;
  unsigned long inside_circle = 0;
  randomSeed(analogRead(A0));
  
  while (Serial.available() > 0) Serial.read();

  while (millis() - pi_start_time < target_duration) {
    unsigned long dot_start = micros();
    long x = random(0, 10001);
    long y = random(0, 10001);
    if ((x * x) + (y * y) <= 100000000L) {
      inside_circle++;
    }
    total_dots++;
    unsigned long dot_end = micros();
    
    if (millis() - last_report_time >= 1000) {
      last_report_time = millis();
      Serial.print(F(" -> Computing Pi... Dots: ")); Serial.println(total_dots);
    }
    if (Serial.available() > 0) break; 
  }
  unsigned long pi_elapsed = millis() - pi_start_time;
  float final_pi = ((float)inside_circle / (float)total_dots) * 4.0;
  float avg_latency = (float)pi_elapsed / (float)total_dots * 1000.0;

  Serial.println(F(" -> Test 1 Complete. Cooling down..."));
  delay(1500);
  Serial.println();
  Serial.println(F("[2/2] Launching Graphical FPS Engine..."));
  delay(1000);

  size_t frame_len = strlen_P(frame_data);
  unsigned long fps_start_time = millis();
  unsigned long total_frames = 0;

  while (Serial.available() > 0) Serial.read();

  while (millis() - fps_start_time < target_duration) {
    int p = 0;
    while (p < frame_len) {
      char chr = pgm_read_byte_near(frame_data + p);
      Serial.print(chr);
      p++;
    }
    total_frames++;

    if (Serial.available() > 0) break;
  }
  unsigned long fps_elapsed = millis() - fps_start_time;
  float elapsed_sec_fps = (float)fps_elapsed / 1000.0;
  float fps = (float)total_frames / elapsed_sec_fps;
  float bytes_per_sec = (float)(total_frames * frame_len) / elapsed_sec_fps;
  float bus_saturation = (bytes_per_sec / 960.0) * 100.0;
  Serial.println(F("\n========================================================================"));
  Serial.println(F("               GenshiOS TOTAL SYSTEM DIAGNOSTIC REPORT                  "));
  Serial.println(F("========================================================================"));
  Serial.println(F("[ UNIT 1: ATmega328P CORE COMPUTE ]"));
  Serial.print(F("  - Total Dots Plotted : ")); Serial.println(total_dots);
  Serial.print(F("  - Average Latency/Dot: ")); Serial.print(avg_latency, 2); Serial.println(F(" microseconds"));
  Serial.print(F("  - Calculated Pi Value: ")); Serial.println(final_pi, 6);
  Serial.println();
  Serial.println(F("[ UNIT 2: SERIAL BUS GRAPHICS LINK ]"));
  Serial.print(F("  - Total Frames Drawn : ")); Serial.println(total_frames);
  Serial.print(F("  - Render FrameRate   : ")); Serial.print(fps, 2); Serial.println(F(" FPS"));
  Serial.print(F("  - Bus Saturation Load: ")); Serial.print(bus_saturation, 1); Serial.println(F(" % (At 9600 baud)"));
  Serial.println();
  Serial.println(F("[ UNIT 3: KERNEL MEMORY SPACE ]"));
  Serial.print(F("  - Free System RAM    : ")); Serial.print(getFreeRam()); Serial.println(F(" Bytes free"));
  Serial.println(F("========================================================================"));
  
  Serial.println();
  Serial.print(F(" > "));
  
  while(Serial.available() > 0) Serial.read();
}

void game_invader() {
  while(Serial.available() > 0) Serial.read();
  Serial.println(F("\n======================================="));
  Serial.println(F("      GenshiOS Realtime TUI Game       "));
  Serial.println(F("======================================="));
  Serial.println(F("  Controls: [a] Left  [d] Right  [q] Quit"));
  Serial.println(F("  Press ANY key to START..."));
  
  while(Serial.available() == 0) delay(10);
  while(Serial.available() > 0) Serial.read();

  const int SCREEN_WIDTH = 20;
  int playerX = SCREEN_WIDTH / 2;
  int enemyX = 0;
  int enemyDirection = 1;
  unsigned long lastUpdate = 0;
  unsigned long enemyUpdate = 0;
  int frameCount = 0;
  int currentFps = 0;
  unsigned long fpsTimer = 0;
  Serial.print(F("\033[2J")); 

  while (true) {
    unsigned long now = millis();
    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input == 'a' && playerX > 0) {
        playerX--;
      }
      else if (input == 'd' && playerX < SCREEN_WIDTH - 1) {
        playerX++;
      }
      else if (input == 'q') {
        Serial.print(F("\033[2J"));
        Serial.println(F("[GAME OVER] Exited by user."));
        Serial.print(F(" > "));
        break;
      }
    }
    if (now - enemyUpdate >= 150) {
      enemyUpdate = now;
      enemyX += enemyDirection;
      if (enemyX >= SCREEN_WIDTH - 1 || enemyX <= 0) {
        enemyDirection *= -1;
      }
    }

    if (now - lastUpdate >= 16) {
      lastUpdate = now;
      frameCount++;
      if (now - fpsTimer >= 1000) {
        currentFps = frameCount;
        frameCount = 0;
        fpsTimer = now;
      }
      Serial.print(F("\033[H"));
      Serial.println(F("+----------------------+"));
      Serial.print(F("| Genshi-Invader v1.0  | FPS: ")); Serial.println(currentFps);
      Serial.println(F("+----------------------+"));
      Serial.print(F("|"));
      for (int i = 0; i < SCREEN_WIDTH; i++) {
        if (i == enemyX) Serial.print(F("👾"));
        else Serial.print(F(" "));
      }
      Serial.println(F("|"));
      for (int row = 0; row < 4; row++) {
        Serial.println(F("|                      |"));
      }
      Serial.print(F("|"));
      for (int i = 0; i < SCREEN_WIDTH; i++) {
        if (i == playerX) Serial.print(F("▲")); // 自機
        else Serial.print(F(" "));
      }
      Serial.println(F("|"));
      Serial.println(F("+----------------------+"));
      Serial.println(F(" [a] Left  [d] Right  [q] Quit"));
    }
  }
}