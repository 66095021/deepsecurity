#!/usr/bin/env python


window_macro_map={

"%SystemRoot%":   'c:\\windows',
"%ProgramFiles%" : 'c:\\Program files',
"%Temp%":  'C:\\Users\\bcheng\\AppData\\Local\\Temp',
"%AppData%":  'C:\\Users\\bcheng\\AppData\\Roaming ',  
"%UserProfile%": ' C:\\Users\\foo',
"%HoneyPotHome": 'C:\\test'

}
pwd_process={

"%SystemRoot%":   'c:\\windows',
"%ProgramFiles%" : 'c:\\Program files',
"%Temp%":  'C:\\Users\\bcheng\\AppData\\Local\\Temp',
"%AppData%":  'C:\\Users\\bcheng\\AppData\\Roaming ',  
"%UserProfile%": ' C:\\Users\\foo'


}



pwd_process_code_map={

"%SystemRoot%":  0, 
"%ProgramFiles%" : 1,
"%Temp%":  2,
"%AppData%":  3,
"%UserProfile%": 4,
"C:\\": 5,
"Others": 6

}

exe_map={
"adobe": "TFT_PDF",
"soffice":"TFT_DOC",
"notepad":"TFT_TEXT"
}
file_type_code_map={
"TFT_EXE":1,
"TFT_DOC":2,
"TFT_PDF":3,
"TFT_IMAGE":4,
"TFT_ZIP":5,
"TFT_RIA":6,
"TFT_MEDIA":7,
"TFT_TEXT":8,
"TFT_MIME":9
}


if __name__== '__main__':
	print pwd_process , file_type_code_map , pwd_process_code_map,exe_map
