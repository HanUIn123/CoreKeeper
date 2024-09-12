// 커맨드 명령어				옵션				원본 파일이 있는 위치			사본 파일을 저장할 위치

xcopy			/y				.\Engine\Header\*.*						.\Reference\Header\

xcopy			/y				.\Engine\System\Bin\System.dll			.\Client\Bin\
xcopy			/y				.\Engine\Utility\Bin\Utility.dll		.\Client\Bin\

xcopy			/y				.\Engine\System\Bin\System.lib			.\Reference\Library\
xcopy			/y				.\Engine\Utility\Bin\Utility.lib		.\Reference\Library\

xcopy			/y				.\Client\Bin\Resource\Texture\Item\*.*			.\Resource\Texture\Item\
xcopy			/y				.\Client\Bin\Resource\Texture\Logo\*.*			.\Resource\Texture\Logo\
xcopy			/y				.\Client\Bin\Resource\Texture\Monster\*.*			.\Resource\Texture\Monster\
xcopy			/y				.\Client\Bin\Resource\Texture\Player\*.*			.\Resource\Texture\Player\
xcopy			/y				.\Client\Bin\Resource\Texture\Shadow\*.*			.\Resource\Texture\Shadow\
xcopy			/y				.\Client\Bin\Resource\Texture\SkyBox\*.*			.\Resource\Texture\SkyBox\
xcopy			/y				.\Client\Bin\Resource\Texture\Terrain\*.*			.\Resource\Texture\Terrain\
xcopy			/y				.\Client\Bin\Resource\Texture\UI\*.*				.\Resource\Texture\UI\
