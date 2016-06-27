#include<stdio.h>
#include<time.h>

/* 土日祝日かどうかを返す関数　
        返り値:平日=0、土曜=1、日曜=2、休日=3、振休=4、祝日=5 */
/*  この関数(含むint Syunbun()、int Syubun())は、
    http://www.h3.dion.ne.jp/~sakatsu/
  の祝日判定ロジック(ＶＢＡ版)を利用して作ったものです。*/

/*
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
_/
_/　CopyRight(C) K.Tsunoda(AddinBox) 2001 All Rights Reserved.
_/　( http://www.h3.dion.ne.jp/~sakatsu/index.htm )
_/
_/　　この祝日マクロは『kt関数アドイン』で使用しているものです。
_/　　このロジックは、レスポンスを第一義として、可能な限り少ない
_/　  【条件判定の実行】で結果を出せるように設計してあります。
_/　　この関数では、２０１６年施行の改正祝日法(山の日)までを
_/　  サポートしています。
_/
_/　(*1)このマクロを引用するに当たっては、必ずこのコメントも
_/　　　一緒に引用する事とします。
_/　(*2)他サイト上で本マクロを直接引用する事は、ご遠慮願います。
_/　　　【 http://www.h3.dion.ne.jp/~sakatsu/holiday_logic.htm 】
_/　　　へのリンクによる紹介で対応して下さい。
_/　(*3)[ktHolidayName]という関数名そのものは、各自の環境に
_/　　　おける命名規則に沿って変更しても構いません。
_/　
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*/

int Syunbun(int yy); /*春分の日を返す関数*/
int Syubun(int yy); /*秋分の日を返す関数*/
int GetHoliday(int yy, int mm, int dd);
int Holiday(const time_t t);

int GetHoliday(int yy, int mm, int dd)
{
	time_t t = time(NULL);
	struct tm *stm1;
	stm1=localtime(&t);
	stm1->tm_year = yy - 1900;
  	stm1->tm_mon  = mm - 1;
  	stm1->tm_mday = dd;
  	stm1->tm_hour = 0;
  	stm1->tm_min  = 0;
  	stm1->tm_sec  = 0;
	
	t = mktime( stm1 );
	
	return Holiday(t);
}
int Holiday(const time_t t)
{
  int yy;
  int mm;
  int dd;
  int ww;
  int r;
  struct tm *Hizuke;
  const time_t SYUKUJITSU=-676976400; /*1948年7月20日*/
  const time_t FURIKAE=103388400; /*1973年04月12日*/
  Hizuke=localtime(&t);
  yy=Hizuke->tm_year+1900;
  mm=Hizuke->tm_mon+1;
  dd=Hizuke->tm_mday;
  ww=Hizuke->tm_wday;
	
  r=0;
  if (ww==6){
    r=1;
  } else if (ww==0){
    r=2;
  }

  if (t<SYUKUJITSU){
    return r;
  }

  switch (mm) {
  case 1:
    if (dd==1){
      r=5; /*元日*/
    } else {
      if (yy>=2000){
        if (((int)((dd-1)/7)==1)&&(ww==1)){
          r=5; /*成人の日*/
        }
      } else {
        if (dd==15){
          r=5; /*成人の日*/
        }
      }
    }
    break;
  case 2:
    if (dd==11){
      if (yy>=1967){
        r=5; /*建国記念の日*/
      }
    } else if ((yy==1989)&&(dd==24)){
      r=5; /*昭和天皇の大喪の礼*/
    }
    break;
  case 3:
    if (dd==Syunbun(yy)){
      r=5; /*春分の日*/
    }
    break;
  case 4:
    if (dd==29){
      if (yy>=2007){
        r=5; /*昭和の日*/
      } else if (yy>=1989){
        r=5; /*みどりの日*/
      } else {
        r=5; /*天皇誕生日*/
      }
    } else if ((yy==1959)&&(dd==10)){
      r=5; /*皇太子明仁親王の結婚の儀*/
    }
    break;
  case 5:
    if (dd==3){
      r=5; /*憲法記念日*/
    } else if (dd==4){
      if (yy>=2007) {
        r=5; /*みどりの日*/
      } else if (yy>=1986) {
          /* 5/4が日曜日は『只の日曜』､月曜日は『憲法記念日の振替休日』(～2006年)*/
          if (ww>1) {
            r=3; /*国民の休日*/
          }
      }
    } else if (dd==5) {
      r=5; /*こどもの日*/
    } else if (dd==6) {
      /* [5/3,5/4が日曜]ケースのみ、ここで判定 */
      if ((yy>=2007)&&((ww==2)||(ww==3))){
        r=4; /*振替休日*/
      }
    }
    break;
  case 6:
    if ((yy==1993)&&(dd==9)){
      r=5; /*皇太子徳仁親王の結婚の儀*/
    }
    break;
  case 7:
    if (yy>=2003){
      if (((int)((dd-1)/7)==2)&&(ww==1)){
        r=5; /*海の日*/
      }
    } else if (yy>=1996){
      if (dd==20) {
        r=5; /*海の日*/
      }
    }
    break;
  case 8:
    if (dd==11){
      if (yy>=2016){
        r=5; /*山の日*/
      }
    }
    break;
  case 9:
    if (dd==Syubun(yy)){
      r=5; /*秋分の日*/
    } else {
      if (yy>=2003) {
        if (((int)((dd-1)/7)==2)&&(ww==1)){
          r=5; /*敬老の日*/
        } else if (ww==2){
          if (dd==Syubun(yy)-1){
            r=3; /*国民の休日*/
          }
        }
      } else if (yy>=1966){
        if (dd==15) {
          r=5; /*敬老の日*/
        }
      }
    }
    break;
  case 10:
    if (yy>=2000){
      if (((int)((dd-1)/7)==1)&&(ww==1)){
        r=5; /*体育の日*/
      }
    } else if (yy>=1966){
      if (dd==10){
        r=5; /*体育の日*/
      }
    }
    break;
  case 11:
    if (dd==3){
      r=5; /*文化の日*/
    } else if (dd==23) {
      r=5; /*勤労感謝の日*/
    } else if ((yy==1990)&&(dd==12)){
      r=5; /*即位礼正殿の儀*/
    }
    break;
  case 12:
    if (dd==23){
      if (yy>=1989){
        r=5; /*天皇誕生日*/
      }
    }
  }

  if ((r<=3)&&(ww==1)){
     /*月曜以外は振替休日判定不要
        5/6(火,水)の判定は上記ステップで処理済
        5/6(月)はここで判定する  */
    if (t>=FURIKAE) {
      if (Holiday(t-86400)==5){    /* 再帰呼出 */
        r=4;
      }
    }
  }
  return r;
}

/*  春分/秋分日の略算式は
    『海上保安庁水路部 暦計算研究会編 新こよみ便利帳』
  で紹介されている式です。 */

/*春分の日を返す関数*/
int Syunbun(int yy){
  int dd;
  if (yy<=1947){
    dd=99;
  } else if (yy<=1979){
    dd=(int)(20.8357+(0.242194*(yy-1980))-(int)((yy-1983)/4));
  } else if (yy<=2099){
    dd=(int)(20.8431+(0.242194*(yy-1980))-(int)((yy-1980)/4));
  } else if (yy<=2150){
    dd=(int)(21.851+(0.242194*(yy-1980))-(int)((yy-1980)/4));
  } else {
    dd=99;
  }
  return dd;
}

/*秋分の日を返す関数*/
int Syubun(int yy){
  int dd;
  if (yy<=1947){
    dd=99;
  } else if (yy<=1979){
    dd=(int)(23.2588+(0.242194*(yy-1980))-(int)((yy-1983)/4));
  } else if (yy<=2099){
    dd=(int)(23.2488+(0.242194*(yy-1980))-(int)((yy-1980)/4));
  } else if (yy<=2150){
    dd=(int)(24.2488+(0.242194*(yy-1980))-(int)((yy-1980)/4));
  } else {
    dd=99;
  }
  return dd;
}