#include "anim.h"
#include "render.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm")

/* ��������� �������� ��� ��������� */
#define MR3_GET_AXIS_VALUE(Axis) \
  (2.0 * (ji.dw ## Axis ## pos - jc.w ## Axis ## min) / \
  (jc.w ## Axis ## max - jc.w ## Axis ## min) - 1.0)

/* ��������� �������� �������� */
mr3ANIM MR3_Anim;

/* ����������� ������� ���������� */
static INT
  MR3_MouseOldX, MR3_MouseOldY;

/* ���������� �������*/       
static INT64
  TimeFreq,  /* ������ ��������� � ������� */
  TimeStart, /* ����� ������ �������� */
  TimeOld,   /* ����� �������� ����� */
  TimePause, /* ����� ������� � ����� */
  TimeFPS;   /* ����� ��� ������ FPS */
static INT
  FrameCounter; /* ������� ������ */


/* ������� ������������� ��������.
 * ���������:
 *   - ���������� ����:
 *       HWND hWnd;
 * ������������ ��������: ���.
 */
VOID MR3_AnimInit( HWND hWnd )
{
  HDC hDC = GetDC(hWnd);
  LARGE_INTEGER li;
  POINT pt;
  
  memset(&MR3_Anim, 0, sizeof(mr3ANIM));

  MR3_Anim.hWnd = hWnd;
  /* �������������� ����� ����� */
  MR3_Anim.hDC = CreateCompatibleDC(hDC);
  MR3_Anim.hBmFrame = CreateCompatibleBitmap(hDC, 30, 30);
  SelectObject(MR3_Anim.hDC, MR3_Anim.hBmFrame);
  MR3_Anim.W = 30;
  MR3_Anim.H = 30;
  MR3_Anim.NumOfUnits = 0;
  ReleaseDC(hWnd, hDC);

  /* ������������� ������� */
  QueryPerformanceFrequency(&li);
  TimeFreq = li.QuadPart;
  QueryPerformanceCounter(&li);
  TimeStart = TimeOld = TimeFPS = li.QuadPart;
  MR3_Anim.IsPause = FALSE;
  FrameCounter = 0;

  /* ������������� ����� */
  GetCursorPos(&pt);
  ScreenToClient(MR3_Anim.hWnd, &pt);
  MR3_MouseOldX = pt.x;
  MR3_MouseOldY = pt.y;        
  GetKeyboardState(MR3_Anim.KeysOld);

} /* End of 'MR3_AnimInit' function */

/* ������� ��������������� ��������.
 * ���������: ���.
 * ������������ ��������: ���.
 */
VOID MR3_AnimClose( VOID )
{
  INT i;

  /* ������������ ������ ��-��� �������� �������� */
  for (i = 0; i < MR3_Anim.NumOfUnits; i++)
  {
    MR3_Anim.Units[i]->Close(MR3_Anim.Units[i], &MR3_Anim);
    free(MR3_Anim.Units[i]);
    MR3_Anim.Units[i] = NULL;
  }

  /* ������� ������� GDI */
  DeleteObject(MR3_Anim.hBmFrame);
  DeleteDC(MR3_Anim.hDC);
} /* End of 'MR3_AnimClose' function */

/* ������� ��������� ��������� ������� ������� ������.
 * ���������:
 *   - ����� ������ ������� ������:
 *       INT W, H;
 * ������������ ��������: ���.
 */
VOID MR3_AnimResize( INT W, INT H )
{
  HDC hDC = GetDC(MR3_Anim.hWnd);

  /* ������������� ������ � ������ ����� */
  DeleteObject(MR3_Anim.hBmFrame);
  MR3_Anim.hBmFrame = CreateCompatibleBitmap(hDC, W, H);
  SelectObject(MR3_Anim.hDC, MR3_Anim.hBmFrame);

  /* ���������� ������� */
  MR3_Anim.W = W;
  MR3_Anim.H = H;

  ReleaseDC(MR3_Anim.hWnd, hDC);
  if (W > H)
    MR3_RndWp = (DBL)W / H * 3, MR3_RndHp = 3;
  else
    MR3_RndHp = (DBL)H / W * 3, MR3_RndWp = 3;

  MatrFrustum(  
} /* End of 'MR3_AnimResize' function */

/* ������� ���������� ����� ��������.
 * ���������: ���.
 * ������������ ��������: ���.
 */
VOID MR3_AnimRender( VOID )
{
  INT i;
  LARGE_INTEGER li;
  POINT pt;

  /*** ���������� input***/
    /* ���������� */
  GetKeyboardState(MR3_Anim.Keys);
  for (i = 0; i < 256; i++)
    MR3_Anim.Keys[i] >>= 7;
  for (i = 0; i < 256; i++)
    MR3_Anim.KeysClick[i] = MR3_Anim.Keys[i] && !MR3_Anim.KeysOld[i];
  memcpy(MR3_Anim.KeysOld, MR3_Anim.Keys, sizeof(MR3_Anim.KeysOld));

  /* ���� */
  /* ������ */
  MR3_Anim.MsWheel = MR3_MouseWheel;
  MR3_MouseWheel = 0;
  /* ���������� ������� */
  GetCursorPos(&pt);
  ScreenToClient(MR3_Anim.hWnd, &pt);
  MR3_Anim.MsX = pt.x;
  MR3_Anim.MsY = pt.y;
  /* ������������� ����������� */
  MR3_Anim.MsDeltaX = pt.x - MR3_MouseOldX;
  MR3_Anim.MsDeltaY = pt.y - MR3_MouseOldY;
  MR3_MouseOldX = pt.x;
  MR3_MouseOldY = pt.y;

  /* �������� */
  if ((i = joyGetNumDevs()) > 0)
  {
    JOYCAPS jc;

    /* ��������� ����� ���������� � �������� */
    if (joyGetDevCaps(JOYSTICKID2, &jc, sizeof(jc)) == JOYERR_NOERROR)
    {
      JOYINFOEX ji;

      /* ��������� �������� ��������� */
      ji.dwSize = sizeof(JOYCAPS);
      ji.dwFlags = JOY_RETURNALL;
      if (joyGetPosEx(JOYSTICKID2, &ji) == JOYERR_NOERROR)
      {
        /* ������ */
        memcpy(MR3_Anim.JButsOld, MR3_Anim.JButs, sizeof(MR3_Anim.JButs));
        for (i = 0; i < 32; i++)
          MR3_Anim.JButs[i] = (ji.dwButtons >> i) & 1;
        for (i = 0; i < 32; i++)
          MR3_Anim.JButs[i] = MR3_Anim.JButs[i] && !MR3_Anim.JButsOld[i];

        /* ��� */
        MR3_Anim.JX = MR3_GET_AXIS_VALUE(X);
        MR3_Anim.JY = MR3_GET_AXIS_VALUE(Y);
        if (jc.wCaps & JOYCAPS_HASZ)
          MR3_Anim.JZ = MR3_GET_AXIS_VALUE(Z);
        if (jc.wCaps & JOYCAPS_HASU)
          MR3_Anim.JU = MR3_GET_AXIS_VALUE(U);
        if (jc.wCaps & JOYCAPS_HASV)
          MR3_Anim.JV = MR3_GET_AXIS_VALUE(V);
        if (jc.wCaps & JOYCAPS_HASR)
          MR3_Anim.JR = MR3_GET_AXIS_VALUE(R);

        if (jc.wCaps & JOYCAPS_HASPOV)
        {
          if (ji.dwPOV == 0xFFFF)
            MR3_Anim.JPOV = 0;
          else
            MR3_Anim.JPOV = ji.dwPOV / 4500 + 1;
        }
      }
    }
  }

  /*** ���������� ������� ***/
  QueryPerformanceCounter(&li);
  MR3_Anim.GlobalTime = (DBL)(li.QuadPart - TimeStart) / TimeFreq;
  MR3_Anim.GlobalDeltaTime = (DBL)(li.QuadPart - TimeOld) / TimeFreq;

  if (!MR3_Anim.IsPause)
    MR3_Anim.DeltaTime = MR3_Anim.GlobalDeltaTime;
  else
  {
    TimePause += li.QuadPart - TimeOld;
    MR3_Anim.DeltaTime = 0;
  }

  MR3_Anim.Time = (DBL)(li.QuadPart - TimePause - TimeStart) / TimeFreq;

  /* ����� �� ��������� ��������� �������� */
  for (i = 0; i < MR3_Anim.NumOfUnits; i++)
    MR3_Anim.Units[i]->Response(MR3_Anim.Units[i], &MR3_Anim);

  /* ������� ���� */
  SelectObject(MR3_Anim.hDC, GetStockObject(DC_BRUSH));
  SelectObject(MR3_Anim.hDC, GetStockObject(NULL_PEN));
  SetDCBrushColor(MR3_Anim.hDC, RGB(0, 0, 0));
  Rectangle(MR3_Anim.hDC, 0, 0, MR3_Anim.W + 1, MR3_Anim.H + 1);

  /* ��������� �������� */
  for (i = 0; i < MR3_Anim.NumOfUnits; i++)
  {
    SelectObject(MR3_Anim.hDC, GetStockObject(DC_BRUSH));
    SelectObject(MR3_Anim.hDC, GetStockObject(DC_PEN));
    SetDCBrushColor(MR3_Anim.hDC, RGB(255, 255, 255));
    SetDCPenColor(MR3_Anim.hDC, RGB(0, 0, 0));

    MR3_Anim.Units[i]->Render(MR3_Anim.Units[i], &MR3_Anim);
  }

  /* ��������� FPS */
  if (li.QuadPart - TimeFPS > TimeFreq)
  {
    static CHAR Buf[100];

    sprintf(Buf, "FPS: %.5f", MR3_Anim.FPS);
    SetWindowText(MR3_Anim.hWnd, Buf);

    MR3_Anim.FPS = FrameCounter / ((DBL)(li.QuadPart - TimeFPS) / TimeFreq);
    TimeFPS = li.QuadPart;
    FrameCounter = 0;
  }

  /* ����� "��������" ����� */
  TimeOld = li.QuadPart;

  FrameCounter++;

} /* End of 'MR3_AnimRender' function */

/* ������� ������ ����� ��������.
 * ���������: ���.
 * ������������ ��������: ���.
 */
VOID MR3_AnimCopyFrame( VOID )
{
  HDC hDC;

  /* ����� ����� */
  hDC = GetDC(MR3_Anim.hWnd);
  BitBlt(hDC, 0, 0, MR3_Anim.W, MR3_Anim.H, MR3_Anim.hDC, 0, 0, SRCCOPY);
  ReleaseDC(MR3_Anim.hWnd, hDC);
} /* End of 'MR3_AnimCopyFrame' function */

/* ������� ���������� � ������� ������� ��������.
 * ���������:
 *   - ����������� ������ ��������:
 *       MR3UNIT *Unit;
 * ������������ ��������: ���.
 */
VOID MR3_AnimAddUnit( mr3UNIT *Unit )
{
  if (MR3_Anim.NumOfUnits < MR3_MAX_UNITS)
  {
    MR3_Anim.Units[MR3_Anim.NumOfUnits++] = Unit;
    Unit->Init(Unit, &MR3_Anim);
  }
} /* End of 'MR3_AnimAddUnit' function */


/* ������� ������������ �/�� �������������� ������
 * � ������ ���������� ���������.
 * ���������: ���.
 * ������������ ��������: ���.
 */
VOID MR3_AnimFlipFullScreen( VOID )
{
  static BOOL IsFullScreen = FALSE; /* ������� ����� */
  static RECT SaveRC;               /* ����������� ������ */

  if (!IsFullScreen)
  {
    RECT rc;
    HMONITOR hmon;
    MONITORINFOEX moninfo;

    /* ��������� ������ ������ ���� */
    GetWindowRect(MR3_Anim.hWnd, &SaveRC);

    /* ���������� � ����� �������� ��������� ���� */
    hmon = MonitorFromWindow(MR3_Anim.hWnd, MONITOR_DEFAULTTONEAREST);

    /* �������� ���������� ��� �������� */
    moninfo.cbSize = sizeof(moninfo);
    GetMonitorInfo(hmon, (MONITORINFO *)&moninfo);

    /* ��������� � ������ ����� */
    rc = moninfo.rcMonitor;

    AdjustWindowRect(&rc, GetWindowLong(MR3_Anim.hWnd, GWL_STYLE), FALSE);

    SetWindowPos(MR3_Anim.hWnd, HWND_TOPMOST,
      rc.left, rc.top,
      rc.right - rc.left, rc.bottom - rc.top,
      SWP_NOOWNERZORDER);
    IsFullScreen = TRUE;
  }
  else
  {
    /* ��������������� ������ ���� */
    SetWindowPos(MR3_Anim.hWnd, HWND_TOP,
      SaveRC.left, SaveRC.top,
      SaveRC.right - SaveRC.left, SaveRC.bottom - SaveRC.top,
      SWP_NOOWNERZORDER);
    IsFullScreen = FALSE;
  }
} /* End of 'MR3_AnimFlipFullScreen' function */

/* ������� ��������� ����� ��������.
 * ���������:
 *   - ���� �����:
 *       BOOL NewPauseFlag;
 * ������������ ��������: ���.
 */
VOID MR3_AnimSetPause( BOOL NewPauseFlag )
{
  MR3_Anim.IsPause = NewPauseFlag;
} /* End of 'MR3_AnimSetPause' function */

/* ������� ������ �� ��������.
 * ���������: ���.
 * ������������ ��������: ���.
 */
VOID MR3_AnimDoExit( VOID )
{
  PostMessage(MR3_Anim.hWnd, WM_CLOSE, 0, 0);
} /* End of 'MR3_AnimDoExit' function */


/* END OF 'ANIM.C' FILE */
