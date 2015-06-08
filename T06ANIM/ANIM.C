#include "anim.h"

/* ��������� �������� �������� */
static mr3ANIM MR3_Anim;

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
  QueryPerformanceCounter(&li);
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
} /* End of 'MR3_AnimResize' function */

/* ������� ���������� ����� ��������.
 * ���������: ���.
 * ������������ ��������: ���.
 */
VOID MR3_AnimRender( VOID )
{
  INT i;

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
 *       mr3UNIT *Unit;
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

/* ������� ������ �� ��������.
 * ���������: ���.
 * ������������ ��������: ���.
 */
VOID MR3_AnimDoExit( VOID )
{
  PostMessage(MR3_Anim.hWnd, WM_CLOSE, 0, 0);
} /* End of 'MR3_AnimDoExit' function */

/* END OF 'ANIM.C' FILE */