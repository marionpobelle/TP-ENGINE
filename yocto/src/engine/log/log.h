#ifndef	__YOCTO_LOG__
#define	__YOCTO_LOG__

#include "engine/utils/types.h"
#include "engine/utils/utils.h"
#include <string>
#include <vector>

using namespace std;

class YLog	
{
	public:
		
		typedef enum {
			USER_INFO,
			USER_ERROR,
			ENGINE_INFO,
			ENGINE_ERROR,
			ENGINE_WARNING
		}MSG_TYPE;

	protected:
		bool _HideEngineLog;
		unsigned long _LastMessageTime;

		inline static YLog * _Instance;
		YLog()
		{
			_HideEngineLog = false;
			_LastMessageTime = GetTickCount();
		}

		virtual void logMsg(MSG_TYPE type, const char *message, bool intro=true, bool ret=true) = 0;
	public:

		static void log(MSG_TYPE type, const char *message, bool intro=true, bool ret=true)
		{
			if(!_Instance)
				return;
			if(_Instance->_HideEngineLog && (type == ENGINE_ERROR || type == ENGINE_INFO))
				return;
			_Instance->logMsg(type,message,intro,ret);
			_Instance->_LastMessageTime = GetTickCount();
		}

		static void showEngineLog(bool show)
		{
			_Instance->_HideEngineLog = !show;
		}

};


#endif