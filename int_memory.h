// Check if file exists. Return true or false
bool file_exists(String filename) {
  filename = "/fs/" + filename;
  WiFiStorageFile file = WiFiStorage.open(filename);
  if (file) {
    return true;
  } else {
    return false;
  }
}

// Read the file content. If the file does not exist, return empty string
String file_read(String filename) {
  String cont = "";           // content of file
  int blen = 128;             // buffer length
  uint8_t buf[blen];          // read buffer

  filename = "/fs/" + filename;
  
  WiFiStorageFile file = WiFiStorage.open(filename);
  if (file) {
    file.seek(0);
    while (file.available()) {
      // Read from file to buffer in chunks of blen characters
      file.read(buf, blen);
      // Append buffer to variable holding the content of the file
      cont += (char*)buf;
    }
  }
  
  return cont;
}

// Write String to file
void file_write(String filename, String text) {
  filename = "/fs/" + filename;
  
  WiFiStorageFile file = WiFiStorage.open(filename);

  // If file exists, delete it
  if (file) {
    file.erase();
  }

  // Write string variable text to file
  file.write(text.c_str(), text.length());
}
