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
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("=========================");
  Serial.println("---Welcome to GenshiOS---");
  Serial.println("=========================");
  Serial.print(" > ");
}

void loop() {
  // put your main code here, to run repeatedly:
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
        if (command == "help") {
          Serial.println(command);
          Serial.print("##How to use GenshiOS##");
          Serial.print("");
          command = "";
          Serial.print("#Apps#");
          Serial.println("");
          Serial.print("If you enter 'cal' in the command line, the calculator will start.");
          Serial.println();
          Serial.print(" > ");
        }
        else if (command == "cal") {
          Serial.println(command);
          command = "";
          calc();
          int garbage = Serial.parseInt();
          garbage = "";
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
        else if (command.startsWith("run ")) {
          Serial.println(command); 
          String target_file = command.substring(4); 
          target_file.trim();
          run_executable(target_file); 
          command = "";
        }
        else {
          Serial.print(command);
          Serial.println();
          Serial.println();
          Serial.print("##command is not found##");
          Serial.println();
          Serial.print(" > ");
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
  Serial.print("enter the value1");
  Serial.println();
  Serial.print(" > ");

  long value1;
  char value1a;
  while(true){
    if (Serial.available() > 0) {
      value1 = Serial.parseInt();
      value1a = Serial.read();
      break;
    }
  }
  Serial.println(value1);
  Serial.println();
  Serial.println("enter the value2");
  Serial.println();
  Serial.print(" > ");
  
  long value2;
  char value2a;
  while (true){
    if (Serial.available() > 0){
      value2 = Serial.parseInt();
      value2a = Serial.read();
      break;
    }
  }
  Serial.println(value2);
  Serial.println();
  Serial.println("enter the operator");
  Serial.println();
  Serial.print(" > ");
  
  char op;
  char opa;
  while (true){
    if (Serial.available() > 0){
      op = Serial.read();
      opa = Serial.read();
      if (op != '\n' && op != '\r') {
        break;
      }
    }
  }
  Serial.println(op); 
  if (op == '+') {
    ans = value1 + value2;
  }
  else if (op == '-') {
    ans = value1 - value2;
  }
  else if (op == '*') {
    ans = value1 * value2;
  }
  else if (op == '/') {
    ans = value1 / value2;
  }
  else {
    ans = 0;
    Serial.println("Error");
  }
  
  Serial.print("Result: ");
  Serial.print(ans); 
  Serial.println();
  Serial.println();
  Serial.print(" > ");
  ans = 0;
  value1 = 0;
  value2 = 0;
  op = ";";
}

void memo(){
  Serial.println("=======================================");
  Serial.println("--- GenshiOS System Memo Ver 1.0 ---");
  Serial.println("=======================================");
  Serial.println();
  Serial.print(" | ");
  
  while (true){
    if (Serial.available() >= 2){
        content = Serial.readStringUntil('\n');
        if (content != "exit genshios system memo" && content != "view history") {
          memo_data += content + '\n';
          Serial.println(content);
        }
        
        if(content == "view history"){
          Serial.println("[History]"); 
          Serial.println(memo_data);
        }
        if (content == "exit genshios system memo"){
          Serial.println("[Exiting Memo App]"); 
          Serial.println();
          Serial.println("Do you want to save this memo as a file? (y/n)");
          Serial.print(" > ");
          
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
              Serial.println("Error: Disk Full! (Max 3 files). Cannot save.");
            } 
            else {
              while(Serial.available() > 0) Serial.read(); 
              Serial.print("Enter file name > ");
              String new_name = "";
              while (true) {
                if (Serial.available() > 0) {
                  new_name = Serial.readStringUntil('\n');
                  new_name.trim();
                  if (new_name != "") {
                    break;
                  }
                }
              }
              if (!new_name.endsWith(".txt") && !new_name.endsWith(".gns")) {
                new_name += ".txt";
              }
              Serial.println(new_name);
              file_names[target_index] = new_name;
              file_contents[target_index] = memo_data;
              file_count++;
              Serial.println("SUCCESS: Saved to GenshiOS Files!");
            }
          }
          memo_data = ""; 
          
          Serial.println();
          Serial.print(" > ");
          break;
        }
        if (content != "exit genshios system memo") {
          Serial.print(" | ");
        }
    }
  }
}

void list_files() {
  Serial.println();
  Serial.println("--- Files in GenshiOS ---");
  if (file_count == 0) {
    Serial.println(" (No files found)");
  }
  else {
    for (int i = 0; i < 3; i++) {
      if (file_names[i] != "") {
        Serial.print(" - ");
        if (file_names[i].endsWith(".gns")) {
          Serial.print("[GNS] ");
        } else {
          Serial.print("[TXT] ");
        }
        Serial.println(file_names[i]);
      }
    }
  }
  Serial.println("-------------------------");
  Serial.println();
  Serial.print(" > ");
}

void cat_file() {
  while(Serial.available() > 0) Serial.read();
  Serial.println();
  Serial.print("Enter file name to read > ");
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
      Serial.print("----- [ "); Serial.print(file_names[i]); Serial.println(" ] -----");
      Serial.print(file_contents[i]);
      Serial.println("-------------------------");
      found = true;
      break;
    }
  }
  if (!found) {
    Serial.println();
    Serial.println("Error: File not found.");
  }
  
  Serial.println();
  Serial.print(" > ");
}

void run_executable(String target) {
  if (!target.endsWith(".gns")) {
    Serial.println();
    Serial.println("Error: Not a GenshiScript (.gns) file.");
    Serial.println();
    Serial.print(" > ");
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
    Serial.println("Error: File not found.");
    Serial.println();
    Serial.print(" > ");
    return;
  }
  Serial.println();
  Serial.print(" Executing [ "); Serial.print(target); Serial.println(" ]...");
  Serial.println("---------------------------------------");

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
      Serial.println("[OS_KERNEL] Hardware LED -> HIGH");
    }
    else if (line == "LED LOW") {
      digitalWrite(ledPin, LOW);
      Serial.println("[OS_KERNEL] Hardware LED -> LOW");
    }
    else {
      Serial.print("Syntax Error: Unknown instruction -> ");
      Serial.println(line);
    }
  }
  Serial.println("---------------------------------------");
  Serial.println("SUCCESS: Process finished with exit code 0.");
  Serial.println();
  Serial.print(" > ");
}