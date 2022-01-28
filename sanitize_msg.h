String sanitize_msg(String raw_msg) {
  String clean_msg = raw_msg;
  int old_pos, pos;

  // If not already done, convet to upper case
  clean_msg.toUpperCase();

  // Clean the message string. German umlauts and szlig will be
  // replaced by AE#, OE#, UE# and SS#. All other non-ASCII
  // character codes will be replaced by ###, which will be
  // eventually ignored by the function display_msg().
  old_pos = -1;
  pos = clean_msg.indexOf("%", old_pos + 1);
  while(-1 != pos) {
    String substr = clean_msg.substring(pos, pos + 3);
    if(substr == "%E4" || substr == "%C4") {
      clean_msg.setCharAt(pos,     'A');
      clean_msg.setCharAt(pos + 1, 'E');
      clean_msg.setCharAt(pos + 2, '#');
    } else if(substr == "%F6" || substr == "%D6") {
      clean_msg.setCharAt(pos,     'O');
      clean_msg.setCharAt(pos + 1, 'E');
      clean_msg.setCharAt(pos + 2, '#');
    } else if(substr == "%FC" || substr == "%DC") {
      clean_msg.setCharAt(pos,     'U');
      clean_msg.setCharAt(pos + 1, 'E');
      clean_msg.setCharAt(pos + 2, '#');
    } else if(substr == "%DF") {
      clean_msg.setCharAt(pos,     'S');
      clean_msg.setCharAt(pos + 1, 'S');
      clean_msg.setCharAt(pos + 2, '#');
    } else {
      for(int i = pos; i < pos + 3; i++) {
        clean_msg.setCharAt(i, '#');
      }        
    }

    // Look for the next umlaut or szlig
    old_pos = pos;
    pos = clean_msg.indexOf("%", old_pos + 1);
  }

  // Return the cleaned message
  return clean_msg;
}
