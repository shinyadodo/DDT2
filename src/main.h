#include <pebble.h>
	
GFont GetCustomFont(uint32_t resource_id);
void handle_init(void);
void handle_tick_timer(struct tm *tick_time, TimeUnits units_changed);
void handle_deinit(void);
void Update_Datetime();
void battery_handler(BatteryChargeState cs);
void bt_handler(bool connected);
int daysInMonth(int mon, int year);
int getWeekday(int nYear, int nMonth, int nDay);
void calendar_layer_update_callback(Layer *me, GContext* ctx);
void setColors(GContext* ctx, GColor Backcolor, GColor Forecolor);
void ChangeColor(int iColor);
void battery_layer_update_callback(Layer *me, GContext* ctx);

char *StrShift( char *String, size_t nShift );
char *StrReplace( char *String, const char *From, const char *To );

//*********************************************************
// 文字列 String の文字列 From を文字列 To で置換する。
// 置換後の文字列 String のサイズが String の記憶領域を超える場合の動作は未定義。
//*********************************************************
char *StrReplace( char *String, const char *From, const char *To )
{
	int   nToLen;   // 置換する文字列の長さ
	int   nFromLen; // 検索する文字列の長さ
	int   nShift;
	char *start;    // 検索を開始する位置
	char *stop;     // 文字列 String の終端
	char *p;

	nToLen   = strlen( To );
	nFromLen = strlen( From );
	nShift   = nToLen - nFromLen;
	start    = String;
	stop     = String + strlen( String );

	// 文字列 String の先頭から文字列 From を検索
	while( NULL != ( p = strstr( start, From ) ) )
	{
		// 文字列 To が複写できるようにする
		start = StrShift( p + nFromLen, nShift );
		stop  = stop + nShift;

		// 文字列 To を複写
		memmove( p, To, nToLen );
	}

	return String;
}//StrReplace
//*********************************************************
// 文字列 String を nShift だけ移動する。
// 移動先へのポインタを返す。
//*********************************************************
char *StrShift( char *String, size_t nShift )
{
	char *start = String;
	char *stop  = String + strlen( String );
	memmove( start + nShift, start, stop-start+1 );

	return String + nShift;
}//StrShift

#define MEIJI  1868
#define TAISHO 1912
#define SHOWA  1926
#define HEISEI 1989

void wareki( struct tm *ltm, char *str)
{
	int y = ltm->tm_year + 1900;	
    char gg[3];	
	
    if( y >= HEISEI ) 
	{
        strcpy( gg, "H." );
		y = y-HEISEI+1;
    } 
	else if( y >= SHOWA ) 
	{
        strcpy( gg, "S." );
		y = y-SHOWA+1;
    } 
	else if( y >= TAISHO ) 
	{
        strcpy( gg, "T." );
		y = y-TAISHO+1;
    } 
	else if( y >= MEIJI ) 
	{
        strcpy( gg, "M." ); 
		y = y-MEIJI+1;
    } 
	else 
	{
        strcpy( gg, "" ); 
    }
	
	snprintf(str, 5, "%s%02d", gg, y);
}

void generate_vibe(uint32_t vibe_pattern_number) 
{
	switch ( vibe_pattern_number ) 
	{
  		case 0: // No Vibration
    		return;
  		case 1: // Single short
    		vibes_short_pulse();
    		break;
		case 2: // Long
			vibes_long_pulse();
			break;
  		case 3: // Double short
			vibes_double_pulse();
			break;
  		default: // No Vibration
    		return;
  	}
}