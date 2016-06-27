#include "pebble.h"
#include "main.h"
#include "Holiday.h"

#define DEVICE_WIDTH        144
#define DEVICE_HEIGHT       168
	
Window *window;
TextLayer *txtDate;
TextLayer *txtampm;
TextLayer *txtTime;
TextLayer *txtbattery;
BitmapLayer *bmpbattery;
GBitmap *imgbattery;

GFont fntTime;

GColor gcBackcolor;
GColor gcForecolor;

BitmapLayer *bmpBluetooth;
GBitmap *imgBluetooth;
GBitmap *imgBluetoothNo;

Layer *calendar_layer;

char abbrDaysOfWeek[7][5] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };


//設定
// define the appkeys used for appMessages
#define AK_BGCOLOR     	1
#define AK_WEEK     	2
#define AK_DATEFORMAT   3
	
#define AK_SUN   		5
#define AK_MON   		6
#define AK_TUE   		7
#define AK_WED   		8
#define AK_THU  		9
#define AK_FRI   		10
#define AK_SAT   		11  

#define AK_AMPM   		12
#define AK_HOLIDAY 		13

#define AK_HOURLY 		14
#define AK_STTIME 		15
#define AK_EDTIME 		16
#define AK_DISCONNECT	17
#define AK_CONNECT 		18

bool mColor;
int ibt;
int iWeek;
char sfmt[] = "%Y/%m/%d\0";
int iampm;
int iHoliday;

int iHourly;
int iSttime;
int iEdtime;
int iDisconnect;
int iConnect;
//設定情報受け取り
static void in_configuration_handler(DictionaryIterator *received, void *context) 
{
	//初期処理
	int bg_color;
	#ifdef PBL_COLOR
		bg_color = 3;
	#else
		bg_color = 1;
	#endif
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ReturnMessage");
	/*色情報取得*/
	Tuple *cfg_bgcolor = dict_find(received, AK_BGCOLOR);
    if (cfg_bgcolor != NULL) 
	{
		bg_color = (int)cfg_bgcolor->value->int32;
    }
	ibt = 0;
	persist_write_int(AK_BGCOLOR, bg_color);
	
	//曜日取得
	Tuple *cfg_week = dict_find(received, AK_WEEK);
    if (cfg_week != NULL) 
	{
		iWeek = (int)cfg_week->value->int32;
    }
	persist_write_int(AK_WEEK, iWeek);
	
	//日付フォーマット
	Tuple *cfg_format = dict_find(received, AK_DATEFORMAT);
    if (cfg_format != NULL) 
	{
        snprintf(sfmt, sizeof(sfmt), "%s", cfg_format->value->cstring);
		StrReplace(sfmt, "Y", "%Y");
		StrReplace(sfmt, "m", "%m");
		StrReplace(sfmt, "d", "%d");
		persist_write_string(AK_DATEFORMAT, sfmt);
    }
		
	//曜日文字
	for(int idx = AK_SUN; idx <= AK_SAT; idx++)
	{
		Tuple *cfg_dayofweek = dict_find(received, idx);
		if (cfg_dayofweek != NULL) 
		{
			strcpy(abbrDaysOfWeek[idx - AK_SUN], cfg_dayofweek->value->cstring);
			persist_write_string(idx, cfg_dayofweek->value->cstring);
		}
	}
	
	//12-24
	Tuple *cfg_ampm = dict_find(received, AK_AMPM);
    if (cfg_ampm != NULL) 
	{
		iampm = (int)cfg_ampm->value->int32;
    }
	persist_write_int(AK_AMPM, iampm);
	
	//Holiday
	Tuple *cfg_holiday = dict_find(received, AK_HOLIDAY);
    if (cfg_holiday != NULL) 
	{
		iHoliday = (int)cfg_holiday->value->int32;
    }
	persist_write_int(AK_HOLIDAY, iHoliday);
	
	//
	Tuple *cfg_hourly = dict_find(received, AK_HOURLY);
    if (cfg_hourly != NULL) 
	{
		iHourly = (int)cfg_hourly->value->int32;
    }
	persist_write_int(AK_HOURLY, iHourly);
	//
	Tuple *cfg_sttime = dict_find(received, AK_STTIME);
    if (cfg_sttime != NULL) 
	{
		iSttime = (int)cfg_sttime->value->int32;
    }
	persist_write_int(AK_STTIME, iSttime);
	//
	Tuple *cfg_edtime = dict_find(received, AK_EDTIME);
    if (cfg_edtime != NULL) 
	{
		iEdtime = (int)cfg_edtime->value->int32;
    }
	persist_write_int(AK_EDTIME, iEdtime);
	//
	Tuple *cfg_disconnect = dict_find(received, AK_DISCONNECT);
    if (cfg_disconnect != NULL) 
	{
		iDisconnect = (int)cfg_disconnect->value->int32;
    }
	persist_write_int(AK_DISCONNECT, iDisconnect);
	//
	Tuple *cfg_connect = dict_find(received, AK_CONNECT);
    if (cfg_connect != NULL) 
	{
		iConnect = (int)cfg_connect->value->int32;
    }
	persist_write_int(AK_CONNECT, iConnect);
	
	ChangeColor(bg_color);
	Update_Datetime();
}
int main(void) 
{
	handle_init();
	app_event_loop();
	handle_deinit();
}

//初期設定
void handle_init(void) 
{
	// Register message handlers
  	app_message_register_inbox_received(in_configuration_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());	
	
	// Create a window and text layer
	window = window_create();
	//Date
	txtDate = text_layer_create(GRect(18, 0, 80, 20));
	text_layer_set_font(txtDate, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	//Time
	txtTime = text_layer_create(GRect(0, 0, 140, 62));
	fntTime = GetCustomFont(RESOURCE_ID_FONT_DIGITAL_BOLD_60);
	text_layer_set_font(txtTime, fntTime);
	text_layer_set_text_alignment(txtTime, GTextAlignmentRight);
	//ampm
	txtampm = text_layer_create(GRect(6, 20, 15, 45));
	text_layer_set_font(txtampm, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(txtampm, GTextAlignmentCenter);
	//battery
	bmpbattery = bitmap_layer_create(GRect(95, 0, 20, 20));
	bitmap_layer_set_compositing_mode(bmpbattery, GCompOpAssign);
	//
	txtbattery = text_layer_create(GRect(109, 0, 35, 20));
	text_layer_set_font(txtbattery, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(txtbattery, GTextAlignmentRight);
	
	//bluetooth
	bmpBluetooth = bitmap_layer_create(GRect(0, 0, 50, 20));
	bitmap_layer_set_alignment(bmpBluetooth, GAlignLeft);
	bitmap_layer_set_compositing_mode(bmpBluetooth, GCompOpAssign);
	
	//Calender
	calendar_layer = layer_create(GRect(0,60,DEVICE_WIDTH,DEVICE_HEIGHT));//slot_bot_bounds);
  	layer_set_update_proc(calendar_layer, calendar_layer_update_callback);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(txtTime));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(txtampm));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bmpBluetooth));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bmpbattery));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(txtbattery));
	layer_add_child(window_get_root_layer(window), calendar_layer);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(txtDate));
	
	ibt = 0;
	ChangeColor(persist_read_int(AK_BGCOLOR));
	
	iWeek = persist_read_int(AK_WEEK);
	persist_read_string(AK_DATEFORMAT, sfmt, sizeof(sfmt));
	if (persist_exists(AK_DATEFORMAT) == false) 
	{
		strcpy(sfmt, "%Y/%m/%d\0");
	}
	iampm = persist_read_int(AK_AMPM);
	iHoliday = persist_read_int(AK_HOLIDAY);
	//曜日文字
	for(int idx = AK_SUN; idx <= AK_SAT; idx++)
	{
		if (persist_exists(idx)) 
		{
			char dof[5];
			persist_read_string(idx, dof, sizeof(dof));
			strcpy(abbrDaysOfWeek[idx - AK_SUN], dof);
		}
	}
	
	iHourly = 0;
	if (persist_exists(AK_HOURLY)) 
	{
		iHourly = persist_read_int(AK_HOURLY);
	}
	iSttime = 0;
	if (persist_exists(AK_STTIME)) 
	{
		iSttime = persist_read_int(AK_STTIME);
	}
	iEdtime = 23;
	if (persist_exists(AK_EDTIME)) 
	{
		iEdtime = persist_read_int(AK_EDTIME);
	}
	iDisconnect = 3;
	if (persist_exists(AK_DISCONNECT)) 
	{
		iDisconnect = persist_read_int(AK_DISCONNECT);
	}
	iConnect = 1;
	if (persist_exists(AK_CONNECT)) 
	{
		iConnect = persist_read_int(AK_CONNECT);
	}
	
	Update_Datetime();
	// Push the window
	window_stack_push(window, true);
	//時間経過での呼び出し処理
	tick_timer_service_subscribe(MINUTE_UNIT|HOUR_UNIT, &handle_tick_timer);
	//バッテリー残量
	battery_state_service_subscribe(&battery_handler);
	//bluetooth
	bluetooth_connection_service_subscribe(&bt_handler);
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}
//色変更処理
void ChangeColor(int iColor)
{
	mColor = false;
	if(iColor == 0)
	{
		#ifdef PBL_COLOR
			mColor = true;
		#else
			iColor = 1;
			gcBackcolor = GColorBlack;
			gcForecolor = GColorWhite;
		#endif
	}
	else if(iColor == 1)
	{
		gcBackcolor = GColorBlack;
		gcForecolor = GColorWhite;
	}
	else if(iColor == 2)
	{
		gcBackcolor = GColorWhite;
		gcForecolor = GColorBlack;
	}
	else if(iColor == 3)
	{
		#ifdef PBL_COLOR
			mColor = true;
		#else
			iColor = 1;
			gcBackcolor = GColorBlack;
			gcForecolor = GColorWhite;
		#endif
	}
	
	#ifdef PBL_COLOR
		if(mColor)
		{
			//GColorCyan
			window_set_background_color(window, GColorCyan);
		
			text_layer_set_background_color(txtDate, GColorCyan);
			text_layer_set_text_color(txtDate, GColorBlack);
		
			text_layer_set_background_color(txtTime, GColorCyan);
			text_layer_set_text_color(txtTime, GColorBlack);
			
			text_layer_set_background_color(txtampm, GColorCyan);
			text_layer_set_text_color(txtampm, GColorBlack);
			
			bitmap_layer_set_background_color(bmpbattery, GColorCyan);
			imgbattery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING_C);
		
			text_layer_set_background_color(txtbattery, GColorCyan);
			text_layer_set_text_color(txtbattery, GColorBlack);
		
			bitmap_layer_set_background_color(bmpBluetooth, GColorCyan);
		
			imgBluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_C);
			imgBluetoothNo = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_NO_C);
		}
		else
		{
			window_set_background_color(window, gcBackcolor);
			
			text_layer_set_background_color(txtDate, gcBackcolor);
			text_layer_set_text_color(txtDate, gcForecolor);
			
			text_layer_set_background_color(txtTime, gcBackcolor);
			text_layer_set_text_color(txtTime, gcForecolor);
			
			text_layer_set_background_color(txtampm, gcBackcolor);
			text_layer_set_text_color(txtampm, gcForecolor);
			
			bitmap_layer_set_background_color(bmpbattery, gcBackcolor);
			
			text_layer_set_background_color(txtbattery, gcBackcolor);
			text_layer_set_text_color(txtbattery, gcForecolor);
			
			bitmap_layer_set_background_color(bmpBluetooth, gcBackcolor);
			if(iColor == 1)
			{
				imgbattery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING_B);
				
				imgBluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_B);
				imgBluetoothNo = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_NO_B);
			}
			else
			{
				imgbattery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING_W);
				
				imgBluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_W);
				imgBluetoothNo = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_NO_W);
			}
		}
	#else
		window_set_background_color(window, gcBackcolor);
	
		text_layer_set_background_color(txtDate, gcBackcolor);
		text_layer_set_text_color(txtDate, gcForecolor);
	
		text_layer_set_background_color(txtTime, gcBackcolor);
		text_layer_set_text_color(txtTime, gcForecolor);
	
		text_layer_set_background_color(txtampm, gcBackcolor);
		text_layer_set_text_color(txtampm, gcForecolor);
	
		bitmap_layer_set_background_color(bmpbattery, gcBackcolor);
	
		text_layer_set_background_color(txtbattery, gcBackcolor);
		text_layer_set_text_color(txtbattery, gcForecolor);
	
		bitmap_layer_set_background_color(bmpBluetooth, gcBackcolor);
		if(iColor == 1)
		{
			imgbattery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING_B);
			
			imgBluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_B);
			imgBluetoothNo = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_NO_B);
		}
		else
		{
			imgbattery = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING_W);
			
			imgBluetooth = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_W);
			imgBluetoothNo = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_NO_W);
		}
	#endif
	//バッテリー残量
	bitmap_layer_set_bitmap(bmpbattery, imgbattery);
	BatteryChargeState cs = battery_state_service_peek();
	battery_handler(cs);
	//bluetooth
	bt_handler(bluetooth_connection_service_peek());
		
}
//時間経過での呼び出し処理
void handle_tick_timer(struct tm *tmdate, TimeUnits units_changes) 
{
	if( units_changes & HOUR_UNIT )
	{
		time_t now = time(NULL); 
  		struct tm *ltm = localtime(&now);
		int iHour = ltm->tm_hour;
		if(iHour >= iSttime && iHour <= iEdtime)
		{
			generate_vibe(iHourly);
		}
	}
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "TickTimer");
	// Get a tm structure
  	Update_Datetime();
}
//現在時間表示
void Update_Datetime()
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Update_Datetime");
	// Get a tm structure
  	time_t now = time(NULL); 
  	struct tm *ltm = localtime(&now);
	
	static char strDate[] = "0000/0000/00";
	static char strTime[] = "00:00";
	
	if(strstr(sfmt,"G")!=NULL)
    {
		static char strGG[] = "H.01\0";
        wareki(ltm, strGG);
		
		char tmp[] = "%y/%m/%d\0";
		static char strDate2[] = "0000/0000/00";
		strcpy( tmp, sfmt );
		StrReplace(tmp, "G", "");
		strftime( strDate2, sizeof(strDate2), tmp, ltm );
		snprintf(strDate, sizeof(strDate), "%s%s", strGG, strDate2);
		
    }
    else
    {
        strftime( strDate, sizeof(strDate), sfmt, ltm );
    }
	
	if(iampm == 0)
	{
		strftime( strTime, sizeof(strTime), "%H:%M", ltm );
		text_layer_set_text(txtampm, "");
		layer_set_hidden((Layer *)txtampm, true);
	}
	else
	{
		strftime( strTime, sizeof(strTime), "%I:%M", ltm );
		static char strampm[] = "A\nM"; 
		strftime( strampm, sizeof(strampm), "%p", ltm );
		strampm[2] = strampm[1];
		strampm[1] = '\n';
		strampm[3] = '\0';
		text_layer_set_text(txtampm, strampm);
		if(strTime[0] == '0')
		{
			strTime[0] = ' ';
		}
		layer_set_hidden((Layer *)txtampm, false);
	}
	
	text_layer_set_text(txtDate, strDate);
	text_layer_set_text(txtTime, strTime);
}
//バッテリー残量
void battery_handler(BatteryChargeState cs) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "BatteryChanged");
 	static char strBattery[16];

	snprintf(strBattery, sizeof(strBattery), "%d%%", cs.charge_percent);
  	if (cs.is_charging) 
	{
		layer_set_hidden((Layer *)bmpbattery, false);
	}
	else
	{
		layer_set_hidden((Layer *)bmpbattery, true);
	}
	text_layer_set_text(txtbattery, strBattery);
	#ifdef PBL_COLOR
		if(mColor == false)
		{
			text_layer_set_text_color(txtbattery, gcForecolor);
		}
		else if(cs.charge_percent >= 60)
		{
			text_layer_set_text_color(txtbattery, GColorDarkGreen);
		}
		else if(cs.charge_percent >= 40)
		{
			text_layer_set_text_color(txtbattery, GColorChromeYellow);
		}
		else
		{
			text_layer_set_text_color(txtbattery, GColorRed);
		}
	#endif
}
//bluetooth
void bt_handler(bool connected) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "BluetoothChanged");
	if (connected) 
	{
		if(ibt == 1) generate_vibe(iConnect);
		bitmap_layer_set_bitmap(bmpBluetooth, imgBluetooth);
	} 
	else 
	{
		generate_vibe(iDisconnect);
		bitmap_layer_set_bitmap(bmpBluetooth, imgBluetoothNo);
	}
	ibt = 1;
}
//
void handle_deinit(void) 
{
	// Destroy the text layer
	text_layer_destroy(txtDate);
	text_layer_destroy(txtTime);
	text_layer_destroy(txtampm);
	text_layer_destroy(txtbattery);
	bitmap_layer_destroy(bmpbattery);
	gbitmap_destroy(imgbattery);
	
	bitmap_layer_destroy(bmpBluetooth);
	gbitmap_destroy(imgBluetooth);
	gbitmap_destroy(imgBluetoothNo);
	// Unload GFont
	fonts_unload_custom_font(fntTime);
	
	layer_destroy(calendar_layer);
	
	bluetooth_connection_service_unsubscribe();
  	battery_state_service_unsubscribe();
  	tick_timer_service_unsubscribe();
	// Destroy the window
	window_destroy(window);
}
//フォント取得
GFont GetCustomFont(uint32_t resource_id)
{
	return fonts_load_custom_font(resource_get_handle(resource_id));
}
// How many days are/were in the month
int daysInMonth(int mon, int year)
{
    // April, June, September and November have 30 Days
    if (mon == 4 || mon == 6 || mon == 9 || mon == 11) {
        return 30;
    } else if (mon == 2) {
        // Deal with Feburary & Leap years
        if (year % 400 == 0) {
            return 29;
        } else if (year % 100 == 0) {
            return 28;
        } else if (year % 4 == 0) {
            return 29;
        } else {
            return 28;
        }
    } else {
        // Most months have 31 days
        return 31;
    }
}
/*
年、月、日を指定すると、曜日を返す。(0:日, ... 6:土)
*/
int getWeekday(int nYear, int nMonth, int nDay)
{
	int nWeekday, nTmp;
	if (nMonth == 1 || nMonth == 2) 
	{
		nYear--;
		nMonth += 12;
	}
	
	nTmp     = nYear/100;
	nWeekday = (nYear + (nYear >> 2) - nTmp + (nTmp >> 2) + (13*nMonth + 8)/5 + nDay) % 7;

	return nWeekday;
}
void setColors(GContext* ctx, GColor Backcolor, GColor Forecolor) 
{
    window_set_background_color(window, Backcolor);
    graphics_context_set_stroke_color(ctx, Forecolor);
    graphics_context_set_fill_color(ctx, Backcolor);
    graphics_context_set_text_color(ctx, Forecolor);
}
void calendar_layer_update_callback(Layer *me, GContext* ctx) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "CalendarChanged");
	time_t now = time(NULL); 
	struct tm *curTime = localtime(&now);
	
	int year = curTime->tm_year + 1900;
    int mon = curTime->tm_mon + 1;
    int day = curTime->tm_mday;
	int weekday = curTime->tm_wday;
    int daysThisMonth = daysInMonth(mon, year);
	
    int calendar[35];
	int thisMonth[35];
	
	//先月データの格納
	int prev_year = year;
	int prev_mon = mon;
	if(mon == 1)
	{
		prev_year--;
		prev_mon = 12;
	}
	else
	{
		prev_mon--;
	}
	int prev_day = daysInMonth(prev_mon, prev_year);
	for(int prevweek = getWeekday(year, mon, 1) - iWeek; prevweek != 0; prevweek--)	
	{
		if(prevweek == -1) prevweek = 6;
		calendar[prevweek - 1]	= prev_day;
		thisMonth[prevweek - 1]	= 0;
		prev_day--;
	}
	//今月データ取得
	int idx = getWeekday(year, mon, 1) - iWeek;
	if(idx == -1) idx = 6;
	for(int thisday = 1; thisday <= daysThisMonth; thisday++)
	{
		if(idx >= 35)
		{
			//6週目以降は1行目に戻る
			calendar[idx - 35]	= thisday;
			thisMonth[idx - 35]	= 1;
		}
		else
		{
			calendar[idx]	= thisday;
			thisMonth[idx]	= 1;
		}
		idx++;
	}
	
	//来月データ取得
	int next_day = 1;
	for(; idx <= 34; idx++)	
	{
		calendar[idx]	= next_day;
		thisMonth[idx]	= 0;
		next_day++;
	}

	// ---------------------------
	// Now that we've calculated which days go where, we'll move on to the display logic.
	// ---------------------------

    #define CAL_DAYS   7   // number of columns (days of the week)
    #define CAL_WIDTH  20  // width of columns
    #define CAL_GAP    1   // gap around calendar
    #define CAL_LEFT   2   // left side of calendar
    #define CAL_HEIGHT 18  // How tall rows should be depends on how many weeks there are

    int weeks  =  5;  // always display 3 weeks: previous, current, next
	
	GFont normal = fonts_get_system_font(FONT_KEY_GOTHIC_14); // fh = 16
	GFont bold   = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD); // fh = 22
	
    GFont current = normal;

    // generate a light background for the calendar grid
	#ifdef PBL_COLOR
		if(mColor)
		{
			setColors(ctx, GColorBlack, GColorCyan);
			graphics_fill_rect(ctx, GRect (CAL_LEFT, CAL_HEIGHT - CAL_GAP, DEVICE_WIDTH - CAL_GAP - CAL_LEFT, CAL_HEIGHT * weeks), 7, GCornersAll);
			setColors(ctx, GColorCyan, GColorBlack);
		}
		else
		{
			setColors(ctx, gcForecolor, gcBackcolor);
			graphics_fill_rect(ctx, GRect (CAL_LEFT, CAL_HEIGHT - CAL_GAP, DEVICE_WIDTH - CAL_GAP - CAL_LEFT, CAL_HEIGHT * weeks), 7, GCornersAll);
			setColors(ctx, gcBackcolor, gcForecolor);
		}
	#else
		setColors(ctx, gcForecolor, gcBackcolor);
		graphics_fill_rect(ctx, GRect (CAL_LEFT, CAL_HEIGHT - CAL_GAP, DEVICE_WIDTH - CAL_GAP - CAL_LEFT, CAL_HEIGHT * weeks), 0, GCornerNone);
		setColors(ctx, gcBackcolor, gcForecolor);
	#endif
		
    for (int col = 0; col < CAL_DAYS; col++) 
	{
		int idx = (col + iWeek) % 7;
		if(idx == curTime->tm_wday)
		{
			current = bold;
		}
		else
		{
			current = normal;
		}
      	// draw the cell background
      	graphics_fill_rect(ctx, GRect (CAL_WIDTH * col + CAL_LEFT + CAL_GAP, 0, CAL_WIDTH - CAL_GAP, CAL_HEIGHT - CAL_GAP), 0, GCornerNone);

      	// draw the cell text
      	graphics_draw_text(ctx, abbrDaysOfWeek[idx], current, GRect(CAL_WIDTH * col + CAL_LEFT + CAL_GAP, CAL_GAP, CAL_WIDTH, CAL_HEIGHT), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL); 

    }
	
	current = normal;
    // draw the individual calendar rows/columns
    int week = 0;
	int dayidx = 0;
	int x1, x2, y1, y2;
    for (int row = 1; row <= 5; row++) 
	{
      	week++;
      	for (int col = 0; col < CAL_DAYS; col++) 
		{
			x1 = CAL_WIDTH * col + CAL_LEFT + CAL_GAP;
			y1 = CAL_HEIGHT * week;
			x2 = CAL_WIDTH - CAL_GAP;
			y2 = CAL_HEIGHT - CAL_GAP;
			if(row == 5)
			{
				//最終行
				y2--;
			}
			//四隅の丸め設定
			int iRound = 0;
			GCornerMask gCorner = GCornerNone;
			#ifdef PBL_COLOR
				//左下
				if(row == 5 && col == 0)
				{
					iRound = 7;
					gCorner = GCornerBottomLeft;
				}
				//右下
				else if(row == 5 && col == 6)
				{
					iRound = 7;
					gCorner = GCornerBottomRight;
				}
				//左上
				else if(row == 1 && col == 0)
				{
					iRound = 7;
					gCorner = GCornerTopLeft;
				}
				//右上
				else if(row == 1 && col == 6)
				{
					iRound = 7;
					gCorner = GCornerTopRight;
				}
			#endif
			if(thisMonth[dayidx] == 0)
			{
				//先月または来月
				#ifdef PBL_COLOR
					if(mColor)
					{
						setColors(ctx, GColorDarkGray, GColorBlack);
					}
					else
					{
						setColors(ctx, gcBackcolor, gcForecolor);
						graphics_fill_rect(ctx, GRect (x1, y1, x2, y2), iRound, gCorner);
						x1 += 1;
						y1 += 1;
						x2 -= 2;
						y2 -= 2;
						setColors(ctx, gcForecolor, gcBackcolor);
					}
				#else
					setColors(ctx, gcBackcolor, gcForecolor);
					graphics_fill_rect(ctx, GRect (x1, y1, x2, y2), 0, GCornerNone);
					x1 += 1;
					y1 += 1;
					x2 -= 2;
					y2 -= 2;
					setColors(ctx, gcForecolor, gcBackcolor);
				#endif
				current = normal;
			}
			else if((col + 7 + iWeek) % 7 == 0)
			{
				//日曜日
				#ifdef PBL_COLOR
					if(mColor)
					{
						setColors(ctx, GColorRed, GColorBlack);
					}
					else
					{
						setColors(ctx, gcBackcolor, gcForecolor);
					}
				#else
					setColors(ctx, gcBackcolor, gcForecolor);
				#endif
				current = normal;
			}
			else if((col + 7 + iWeek) % 7 == 6)
			{
				//土曜日
				#ifdef PBL_COLOR
					if(mColor)
					{
						setColors(ctx, GColorBlueMoon, GColorBlack);
					}
					else
					{
						setColors(ctx, gcBackcolor, gcForecolor);
					}
				#else
					setColors(ctx, gcBackcolor, gcForecolor);
				#endif
				current = normal;
			}
			#ifdef PBL_COLOR
			else if(iHoliday == 1 && mColor)
			{
				//祝日判定をする(0:平日,1:土曜,2:日曜,3:休日,4:振休,5:祝日)
				if(GetHoliday(year, mon, calendar[dayidx]) >= 3)
				{
					setColors(ctx, GColorRed, GColorBlack);
					current = normal;
				}
			}
			#endif
			
			//カラーの場合は右下と左下を丸める
			// draw the cell background
			graphics_fill_rect(ctx, GRect (x1, y1, x2, y2), iRound, gCorner);
			
			
			if (thisMonth[dayidx] == 1 && calendar[dayidx] == day) 
			{
				//今月かつ日が一致した場合
				x1 += 1;
				y1 += 1;
				x2 -= 2;
				y2 -= 2;
				#ifdef PBL_COLOR
					if(mColor)
					{
						setColors(ctx, GColorMidnightGreen, GColorWhite);
					}
					else
					{
						setColors(ctx, gcForecolor, gcBackcolor);
					}				
				#else
					setColors(ctx, gcForecolor, gcBackcolor);
				#endif
				graphics_fill_rect(ctx, GRect (x1, y1, x2, y2), 8, GCornersAll);
          		//current = bold;
			}
				
	        char date_text[3];
			// draw the cell text
			snprintf(date_text, sizeof(date_text), "%d", calendar[dayidx]);
        	graphics_draw_text(ctx, date_text, current, GRect(CAL_WIDTH * col + CAL_LEFT, CAL_HEIGHT * week - CAL_GAP, CAL_WIDTH, CAL_HEIGHT), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL); 

			#ifdef PBL_COLOR
				if(mColor)
				{
					setColors(ctx, GColorCyan, GColorBlack);
				}
				else
				{
					setColors(ctx, gcBackcolor, gcForecolor);
				}
			#else
				setColors(ctx, gcBackcolor, gcForecolor);
			#endif
          	current = normal;
			
			dayidx++;
		}
	}
}