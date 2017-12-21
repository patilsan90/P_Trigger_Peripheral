
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <JSONParser.h>

int JSONParser ::parse(char str[])
{

  uint8_t i = 0;
  uint8_t len = 0;
  uint8_t k = 0;
  uint8_t start_index;
  JSONParser::total_pairs = 0;
  int infinity_detector_counter = 0;
  while ('\0' != str[i])
  {
    infinity_detector_counter++;
    if (infinity_detector_counter >= 30)
      return -1;
    if ('\"' == str[i]) //start of key
    {
      i++;
      while (str[i] == ' ')
      {
        i++;
        infinity_detector_counter++;
        if (infinity_detector_counter >= 30)
          return -1;
      }

      start_index = i;
      while (str[i] != '\"')
      {
        len++;
        i++;
        infinity_detector_counter++;
        if (infinity_detector_counter >= 30)
          return -1;
      }
      JSONParser::pairs[k].key = (char *)malloc((len + 1) * sizeof(char));
      strncpy(JSONParser::pairs[k].key, str + start_index, len);
      JSONParser::pairs[k].key[len] = '\0';
      len = 0;
    }

    if (str[i] == ':') //start of value
    {
      i++;
      while (str[i] == ' ' || str[i] == '"')
      {
        i++;
        infinity_detector_counter++;
        if (infinity_detector_counter >= 30)
          return -1;
      }
      start_index = i;
      while (str[i] != '"' && str[i] != ',' && str[i] != '}')
      {
        len++;
        i++;
            infinity_detector_counter++;
    if (infinity_detector_counter >= 30)
      return -1;

      }

      JSONParser::pairs[k].val = (char *)malloc((len + 1) * sizeof(char));
      strncpy(JSONParser::pairs[k].val, str + start_index, len);
      JSONParser::pairs[k].val[len] = '\0';
      len = 0;
    }

    if (',' == str[i])
    {
      k++;
      JSONParser::total_pairs++;
    }
    i++;
  }

  JSONParser::total_pairs += 1;

  /*  for (i = 0; i < JSONParser::total_pairs; i++)
  {
    pairs[i].val = trimwhitespace(pairs[i].val);
    pairs[i].key = trimwhitespace(pairs[i].key);
  }*/
  return 0;
}

/*char *JSONParser ::trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while (isspace((unsigned char)*str))
    str++;

  if (*str == 0) // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  // Write new null terminator
  *(end + 1) = 0;

  return str;
}*/