/*
 * This function scans the http request for the pattern
 * [?&]<param>=<value> and returns value as a string.
 * 
 * The http request has the form
 * GET?<param1>=<value1>&<param2>=<value2> HTTP/1.1
 */
String scan_request(String request, String param) {
  // Define variables
  int start_pos, end_pos, ws_pos;
  String value = "";

  // Nothing to if the request is not complete, i.e. does not end with
  // "HTTP/1.1"
  if (request.endsWith("HTTP/1.1")) {
  
    // See if the request contains "?<param>=" or "&<param>=" and determine
    // the position within the request. If it is not contained, the
    // indexOf() function will return -1
    start_pos = max(
      request.indexOf("?" + param + "="),
      request.indexOf("&" + param + "=")
      );
  
    // If start_pos is not -1 then extract the value for the parameter that follows
    // "?<param>=" or "&<param>=". The end of the value is either defined by the next
    // following character "&" or " " (end of the HTTP request). Needless to
    // mention that the actual value will never contain these characters
    // because they are encoded.
    if (-1 != start_pos) {
      // First, set end_pos to the position of the next character "&"
      end_pos = request.indexOf("&", start_pos + 1);
      // If end_pos is -1 there is no following "&" and the value ends with the end of
      // the request (character " "):
      if (-1 == end_pos) {
        end_pos = request.indexOf(" ", start_pos + 1);
      }
      // If end_pos is not -1 then we can extract the value. The begin of the
      // value is located at startpos + <param>.length() + 2. The two results
      // from the characters "?" or "&" ahead of <param> as well as the character
      // "=" after <param>.
      if (-1 != end_pos) {
        value = request.substring(start_pos + param.length() + 2, end_pos);
        Serial.print(param);
        Serial.print(" --> ");
        Serial.println(value);
      }
    }
  }

  return value;
}
