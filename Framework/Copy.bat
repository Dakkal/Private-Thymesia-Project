// 명령어		옵션		원본파일 위치					사본파일 저장 위치
	
xcopy		/y		.\Engine\Public\*.h						.\Reference\Headers\		
xcopy		/y		.\Engine\Bin\*.lib						.\Reference\Librarys\Debug\		
xcopy		/y		.\Engine\Bin\*.dll						.\Client\Bin\

xcopy		/y		.\Engine\BinR\*.lib						.\Reference\Librarys\Release\		
xcopy		/y		.\Engine\BinR\*.dll						.\Client\BinR\