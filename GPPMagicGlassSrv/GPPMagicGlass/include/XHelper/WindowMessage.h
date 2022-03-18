#pragma once
#include <Windows.h>
//定义窗口的自定义窗口消息

//按模块来区分消息范围(增加一个模块 范围增加2000)

//游戏加加（WM_USER + 1  ---  WM_USER + 1999）
typedef enum _GAMEPP_MESSAGE {
	GAMEPP_WM_REFRESH_INFO = WM_USER + 1,       //刷新
	GAMEPP_WM_SWITCH_INFO,                      //切换加载画质优化方案
	GAMEPP_WM_OPEN_INFO,                        //开启画质效果
	GAMEPP_WM_QUIT_SAVE,                        //进程退出
	GAMEPP_WM_SWITCH_HARDWARE,                  //热键切换硬件显示信息
	GAMEPP_WM_HARDWARE_TAB,                     //显示硬件tab
	GAMEPP_WM_RESHADE_TAB,					    //显示画质tab
	GAMEPP_WM_SAVE_VIDEO,					    //视频文件保存成功
	GAMEPP_WM_SAVE_SCREEN_PICTURE,			    //截图文件保存成功
	GAMEPP_WM_GAME_INIT,                        //有游戏进入
	GAMEPP_WM_SHOW_KEY_DOWN,                    //锁定热键触发
	GAMEPP_WM_LOADING_START,                    //加载画质方案
	GAMEPP_WM_GAME_MOUSESPEED_CUT,              //减少鼠标速度
	GAMEPP_WM_GAME_MOUSESPEED_ADD,              //增加鼠标速度
	GAMEPP_WM_GAME_VOLUME_CUT,                  //减小音量
	GAMEPP_WM_GAME_VOLUME_ADD,                  //增加音量
	GAMEPP_WM_ANIMATION_FRAME_SHOW,             //显示统计界面
	GAMEPP_WM_ANIMATION_WINDOW_SIZE,            //重置统计界面大小
	GAMEPP_WM_ANIMATION_FRAME_HIDE,             //关闭统计界面
	GAMEPP_WM_RELOAD_RESHADE,                   //重置加载方案
	GAMEPP_WM_SWITCH_HARDWARE_INFO,             //设置当前是使用横向显示 或者 纵向显示
	GAMEPP_WM_LOADING_STOP,                     //停止画质方案
	GAMEPP_WM_PROCESS_QUIT,                     //进程退出
	GAMEPP_WM_SHOW_AND_CHANGE_MODAL,            //显示当前的选择画质方案
	GAMEPP_WM_HIDE_WINDOW,                      //隐藏窗体
	CAMEPP_WM_SCREENHOT_MANUALLY,               //手动截图
	CAMEPP_WM_SCREENHOT_AUTO,                    //自动截图
	CAMEPP_WM_PROGRAM_CHANGE,                    //画质方案有改变
	CAMEPP_WM_SHOW_INFO_MESSAGE,                 //游戏端信息更新
	GAMEPP_WM_OPEN_INFO2,                        //开启画质效果
	GAMEPP_WM_LANGUAGE_CHANGE,          //语言选择有变化
	GAMEPP_WM_FONT_SIZE_CHANGE,          //字体大小发生变化
	GAMEPP_WM_POS_CHANGE,                      //窗体位置发生变化
	GAMEPP_WM_GAME_WINDOW_SIZE_CHANGE,  //游戏窗体大小发生变化
	GAMEPP_WM_HW_ENABLE_UI,				//硬件开关提示界面

	GAMEPP_BARRAGE_SET_POS,                 //弹幕重置位置
	GAMEPP_BARRAGE_SET_VISIBLE,            //弹幕重置状态
	GAMEPP_BARRAGE_UPDATE,                   //弹幕重置信息

	GAMEPP_BRAARAGE_COMMENT_STATE,       //弹幕网络链接状态
	GAMEPP_BRAARAGE_QUIT,                           //弹幕关闭
	GAMEPP_BRAARAGE_ACTION_STATE,           //弹幕显示状态
	GAMEPP_NOTIFY_LOCK_MESSAGE,              //锁定状态通知消息

	GAMEPP_BRAARAGE_OPEN,             //弹幕开启
	GAMEPP_BRAARAGE_CLOSE,            //弹幕关闭
	GAMEPP_BRAARAGE_WINDOW_STATE_CHANGE,            //游戏窗体状态有改变
	GAMEPP_BRAARAGE_HOTKEY_ACTIVE,             //弹幕热键触发

	GAMEPP_BRAARAGE_BACKGROUND_TRANSPARENT,
	GAMEPP_BRAARAGE_BACKGROUND_COLOR,
	GAMEPP_BRAARAGE_SHOW_HOTKEY_INFO,
	GAMEPP_BRAARAGE_HOTKYE_TIPS_SWITCH,
	GAMEPP_BRAARAGE_SHOW_HOTKEY,
	GAMEPP_BRAARAGE_RE_CONNECT,

	GAMEPP_BRAARAGE_AFTER_CREATE_WINDOW,         //弹幕创建窗体成功

	CAMEPP_WM_PROGRAM_CHANGE_FOUCS,                    //画质方案有改变

	GAMEPP_WM_WALLPAPER_SWITCH,
	GAMEPP_WM_WALLPAPER_REFRESH,          //更新浮窗显示
	GAMEPP_WM_WALLPAPER_LOCK,

	GAMEPP_TERMINATE_FRAME_SHOW,
	GAMEPP_TERMINATE_FRAME_HIDE,
	GAMEPP_INITHARDWARE_FINISH,
	GAMEPP_HIGLIGHT_RELIVE,              //relive 通知
	GAMEPP_DIRECTOR_CONFIG_CHANGE,       //导演模式 配置有更新
	GAMEPP_DIRECTOR_OPEN_CHANGE,       //导演模式 开关有更新
	GAMEPP_QUALITY_CHANGE,         //画质设置有更新
	CAMEPP_BARRAGE_CONFIG_CHANGE,  //弹幕配置更新
	GAMEPP_MAIN_FRAME_VISIBLE_CHANGE,

	GAMEPP_DIGITAL_VIBRATION_CHANGE,
	GAMEPP_HOTKEYSSHIELD_CONFIG_CHANGE,
	GAMEPP_HARDWARE_CONFIG_CHANGE,
	GAMEPP_MAINFRAME_DIRECTOR_CONFIG_CHANGE,       //导演模式 配置有更新
	GamePP_WM_OPTIMIZE_START,
	CAMEPP_NOTE_SHOW_ID_CHANGE,
	CAMEPP_RECORD_START_TIP,
	CAMEPP_RECORD_STOP_TIP,
	GAMEPP_DESKTOP_UNLOCK,
	GAMEPP_OCR_HOTKEY,
	GAMEPP_WM_WALLPAPER_LOCK2,
	GAMEPP_WM_CLOSE_SOME_MODULE,
}GAMEPP_MESSAGE;

//吃鸡助手（WM_USER + 2000  ---  WM_USER + 3999）
typedef enum _PUBG_MESSAGE {

	PUBG_WM_REFRESH_INFO = WM_USER + 2000,    //更新消息
	PUBG_WM_QUIT_SAVE,                        //进程退出
	PUBG_WM_GAME_INIT,                        //有游戏进入
	PUBG_WM_RECORDFRAME,                      //显示游戏内战绩界面
	PUBG_WM_SHOW_KEY_DOWN,                    //锁定热键触发
	PUBG_WM_NEW_USER_BIND,                    //新用户绑定查询

	PUBG_WM_VISION_FIRST,                     //响应1号队员热键
	PUBG_WM_VISION_SECOND,                    //响应2号队员热键
	PUBG_WM_VISION_THIRD,                     //响应3号队员热键
	PUBG_WM_VISION_FOURTH,                    //响应4号队员热键

	PUBG_WM_VISION_REFRESH,                   //图片更新
	PUBG_WM_VISION_DISBAND,					  //房间解散
	PUBG_WM_VISION_OPEN,					  //勇士系统开启
	PUBG_WM_VISION_CHANGE,                    //切换展示图像热键
	PUBG_WM_VISION_SECREENHOT,                //有截图产生

	PUBG_WM_IPPING_VAILSIBLE,                 // 
	PUBG_WM_WEBSOCKET_CONNECT,				  //有链接
	PUBG_WM_IPPING_LSP,
	PUBG_WM_PUSH_MESSAGE,					  //有推送信息
	PUBG_WM_HIDE_WINDOW,					  //关闭显示窗体
	PUBG_WM_GAME_WINDOW_THREAD,				  //找到游戏窗体的线程id
	PUBG_WM_MAP_UPDATE_SIZE,				  //地图窗体接收游戏窗体改变的消息
	PUBG_WM_MAPFRAME,

	PUBG_WM_RECORDE_NOTIFY,					   //战绩更新
	PUBG_WM_QUERY_DONE,                        //战绩查询结束
	PUBG_WM_QUERY_UPDATE_TIME,				   //战绩更新
	PUBG_WM_PROCESS_QUIT,				       //进程退出
	PUBG_WM_NOT_KEEP_ALIVE,				       //对应插件主程序不存在，但render进程不退出

	PUBG_WM_GAME_PROCESS_INIT,				   //第一次加载游戏
	PUBG_WM_GAME_PROCESS_CONNECT,			   //重新连接上游戏

	PUBG_WM_VISION_VOICE_REFRESH,              //语音更新
	
	PUBG_WM_GAME_OVER,                         //当场游戏对局结束
	PUBG_WM_GET_STEAMID,                       //获取到steamid

	PUBG_WM_VISION_SECREENHOT_SAVE,             //已保存截图

	PUBG_WM_VISION_CENTER_TIMEOUT,             //截图显示时间已到

}PUBG_MESSAGE;


//render 模块内部消息
typedef enum _RENDER_MESSAGE {
	RENDER_TIPS_CONTENT_CHANGE = WM_USER + 4000,     //更新提示消息
	RENDER_PROCESS_QUIT,                             //进程退出
	RENDER_WM_RUM_CORE,                              //是否要加载模块的消息
	RENDER_BARRGE_WINDOW_SHOW,             //弹幕加载
	RENDER_TIPS_WINDOW_HIDE,                     //隐藏提示窗体（避免被截图保存下来）
	RENDER_CLIENTACTIONDISPATCH_INIT,         //客户端初始化消息
}RENDER_MESSAGE;

typedef enum _FORTNITE_MESSAGE {
	FORTNITE_TIPS_CONTENT_CHANGE = WM_USER + 4000,     //更新提示消息
	FORTNITE_PROCESS_QUIT,                             //进程退出
}FORTNITE_MESSAGE;

//定义设置快捷键的id
//自定义热键id,
typedef enum _HOTKEYID {

	HOTKEY_MOUSE_CUT_ID = 100,			    //减少鼠标速度
	HOTKEY_MOUSE_ADD_ID,					//增加鼠标速度
	HOTKEY_MUTE_ID,								//静音
	HOTKEY_VOLUME_CUT_ID,					//音量减少
	HOTKEY_VOLUME_ADD_ID,					//音量增加
	HOTKEY_SHOW_UI_ID,						//切换显示游戏内主界面
	HOTKEY_SHOU_INFO_ID,						//切换显示信息
	HOTKEY_ESC_ID,									//ESC键
	HOTKEY_REGIONAL_SCREENSHOT_ID,          //截图指令
	HOTKEY_VIDEO_ID,						//录像
	HOTKEY_REVERSE_VIDEO_ID,				//记录按键之前视频
	HOTKEY_SWITCH_PROGRAM,				    //画质优化方案切换
	HOTKEY_OPEN_PROGRAM,				    //画质优化方案开启
	HOTKEY_SHOU_HARDWARE_INFO_ID,			//切换显示硬件信息
	HOTKEY_RECORDE_NORMAL,                  //正常录像
	HOTKEY_RECORDE_BACKTRACK,               //回溯录像
	HOTKEY_SCREEN_NORMAL,                      //正常截图（F9）
	HOTKEY_SHOW_RESHADE_OPTI,               //显示画质优化界面

	HOTKEY_SCREEN_ID = 200,			//勇士系统发送视觉截图
	HOTKEY_SHOU_RECORDFRAME_ID,	    //显示界面内战绩
	HOTKEY_SHOU_VISION_F1,			//f1
	HOTKEY_SHOU_VISION_F2,			//f2
	HOTKEY_SHOU_VISION_F3,			//f3
	HOTKEY_SHOU_VISION_F4,			//f4
	HOTKEY_SHOW_TOGGLE,			    //关闭/显示下一个
	//HOTKEY_SHOW_MAPS,               //地图
	HOTKEY_CAPTURE_VOICE,           //语音热键
	HOTKEY_PLAY_SPECIFY_VOICE,      //显示语音播放界面
	HOTKEY_RECORD_ANALYSISDATA,     //战报界面

	HOTKEY_PUBG_TEAM_SYSTEM,         //pubg++组队
	HOTKEY_STOP_SPECIFY_VOICE,       //停止正在播放的语音

	HOTKEY_SHOW_BARRGE,       //游戏内弹幕显示
	
	HOTKEY_TFT_SHOW_ALL,
	HOTKEY_TFT_SETTING,

	HOTKEY_TERMINATE_PROCESS,   //Alt+F4

	HOTKEY_SWITCH_HIGHLIGHT, //游戏内弹幕显示

	HOTKEY_OPTIMIZE_CANCLE,  //取消优化

	HOTKEY_PERFORMANCE_RECORD,  //记录截取
	HOTKEY_NOTE_SHOW_OR_HIDE,  //笔记显示或隐藏

	HOTKEY_IMG_RECOGNITION,  //图像识别按键
}HOTKEYID;

//键值存放位
#define CONTROLKEY  0x10000000
#define SHIFTKEY    0x20000000
#define ALTKEY		0x40000000
#define WINKEY		0x80010000


//提示窗体全部整合成一个，所以想样式全部为 图标 +　文字（有可能是一行或者为两行）
typedef enum  _TIPS_CONTEXT_TYPE {
	CONTEXT_NULL = 0,
	CONTEXT_SIGLELINE,              //只有一行文字     
	CONTEXT_MUTILLLINE,           //多行文字（也只有两行）
}TIPS_CONTEXT_TYPE;

#define TIME_HIGHLIGHT_ID  100



