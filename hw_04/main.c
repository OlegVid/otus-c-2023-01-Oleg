/* homework for course c-2023-01 lesson No 9
 *author: Oleg Videnin
 *date:  24/04/2023
 *Чтение файла из аргумента вызова и вывод списка слов в формате: слово = количество_встреч_в_тексте
 *
*/
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <json.h>




struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

void pri_short_help_msg(char *name) {
    printf("usage: %s town\n", name);
}


static const char * win_dir_conv(const char *windir);

void print_parsed_json_hourly_data(char *description, struct json_object * j_hourly_element ){
    struct json_object *j_tempC;
    struct json_object *j_weatherDesc;
    struct json_object *j_weatherDesc_element;
    struct json_object *j_weather_value;
    struct json_object *j_winddir16Point;
    struct json_object *j_windspeedKmph;

	  json_object_object_get_ex(j_hourly_element,"tempC",&j_tempC);
	  json_object_object_get_ex(j_hourly_element,"winddir16Point",&j_winddir16Point);
	  json_object_object_get_ex(j_hourly_element,"windspeedKmph",&j_windspeedKmph);

          json_object_object_get_ex(j_hourly_element,"lang_ru",&j_weatherDesc);  // extract weathe description array
	  j_weatherDesc_element = json_object_array_get_idx(j_weatherDesc, 0);	
	  json_object_object_get_ex(j_weatherDesc_element,"value",&j_weather_value);	

          printf("%s ",description);
	  printf("Температура: %s C, ", json_object_get_string(j_tempC));
          printf("Ветер: %s ",win_dir_conv(json_object_get_string(j_winddir16Point)));
          printf("%s км/час, ",json_object_get_string(j_windspeedKmph));
          printf("%s.\n", json_object_get_string(j_weather_value));	
}

void print_parsed_json_wttr_data(char *json_data){
// variables for json obj
    struct json_object *j_nearest_area;
    struct json_object *j_areaName;
    struct json_object *j_areaValue;
    struct json_object *j_wttr;
    struct json_object *j_weather;
    struct json_object *j_weather_element;
    struct json_object *j_date;
    struct json_object *j_hourly;
    struct json_object *j_hourly_element;

    struct json_object *j_time;


	j_wttr = json_tokener_parse(json_data); 				//first parse
	
	json_object_object_get_ex(j_wttr,"nearest_area",&j_nearest_area);        //extract nearest area array 
        j_nearest_area = json_object_array_get_idx(j_nearest_area, 0);
	json_object_object_get_ex(j_nearest_area,"areaName",&j_areaName); 
  	j_areaName = json_object_array_get_idx(j_areaName, 0);
	json_object_object_get_ex(j_areaName,"value",&j_areaValue); 

	json_object_object_get_ex(j_wttr,"weather",&j_weather); 		//extract weather array 
	j_weather_element = json_object_array_get_idx(j_weather, 0);		// extract first day (according to task) 
	json_object_object_get_ex(j_weather_element,"date",&j_date);            // extract date obj
	json_object_object_get_ex(j_weather_element,"hourly",&j_hourly);       // extract hourly weather array
	size_t len = json_object_array_length(j_hourly);                       // extract hourly weather array length (8 - now) but can change

 	printf("Прогноз погоды на: %s  %s \n",json_object_get_string(j_date), json_object_get_string(j_areaValue));

	for(size_t i = 0;i < len; i++){

		j_hourly_element = json_object_array_get_idx(j_hourly, i);       // get i element of hourly array
		json_object_object_get_ex(j_hourly_element,"time",&j_time);     // extract time obj, we will print only night, morning, day, evning values
	   
		if (strcmp(json_object_get_string(j_time),"300") == 0){
	  	 print_parsed_json_hourly_data("Ночь: ",j_hourly_element);
		}

		if (strcmp(json_object_get_string(j_time),"900") == 0){
	  		 print_parsed_json_hourly_data("Утро: ",j_hourly_element);
		}

		if (strcmp(json_object_get_string(j_time),"1200") == 0){
	  		 print_parsed_json_hourly_data("День: ",j_hourly_element);
		}


		if (strcmp(json_object_get_string(j_time),"1800") == 0){
	  		 print_parsed_json_hourly_data("Вечер:",j_hourly_element);
		}

	}
}


int main(int argc, char **argv) {


  struct MemoryStruct chunk;
 
  chunk.memory = malloc(1);  
  chunk.size = 0;    

   CURL *curl = curl_easy_init();
   if (!curl)
    {
        fprintf(stderr,"[-] Failed Initializing Curl\n");
        exit(-1);
    }

    
    if ((argc != 2) || (argv[1] == NULL)) {
        pri_short_help_msg(argv[0]);
        return 0;
    }

    CURLcode res;
    char * url = calloc(strlen("https://ru.wttr.in/?format=j1") + strlen(argv[1]) + 2,sizeof(char));
    sprintf(url,"https://ru.wttr.in/%s?format=j1",argv[1]);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); 
	  /* send all data to this function  */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	  /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl);
  
    if (res != CURLE_OK)
    {
        fprintf(stderr,"[-] Could Not Fetch Webpage\n[+] erro: %d, Error : %s\n",res, curl_easy_strerror(res));
        exit(-2);
    }

	print_parsed_json_wttr_data(chunk.memory); 
    curl_easy_cleanup(curl);


    return 0;
}

const char *win_dir_conv(const char *windir){
  static char result[60];
  strncpy(result,"н/д", sizeof(result));
  if (strcmp(windir,"N") == 0){
    strncpy(result,"Северный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"NNE") == 0){
    strncpy(result,"Северо-северо-восточный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"ENE") == 0){
    strncpy(result,"Востоко-северо-восточный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"NE") == 0){
    strncpy(result,"Северо-восточный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"E") == 0){
    strncpy(result,"Восточный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"ESE") == 0){
    strncpy(result,"Востоко-юго-восточный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"SE") == 0){
    strncpy(result,"Юго-восточный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"SSE") == 0){
    strncpy(result,"Юго-юго-восточный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"S") == 0){
    strncpy(result,"Южный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"SSW") == 0){
    strncpy(result,"Юго-юго-западный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"SW") == 0){
    strncpy(result,"Юго-западный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"WSW") == 0){
    strncpy(result,"Западо-юго-западный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"W") == 0){
    strncpy(result,"Западный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"WNW") == 0){
    strncpy(result,"Западо-северо-западный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"NW") == 0){
    strncpy(result,"Северо-западный", sizeof(result));
    return result;
  }
  if (strcmp(windir,"NNW") == 0){
    strncpy(result,"Северо-северо-западный", sizeof(result));
    return result;
  }
  return result;
}

