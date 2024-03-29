#include <string.h>
#include <time.h>

#include "anim.h"
#include "vec.h"

/* ��� ������������� ���� */
typedef struct tagMR3UNIT_BALL
{
  MR3_UNIT_BASE_FIELDS;

  VEC Pos;     /* ������� ���� */
  DWORD Color; /* ���� ���� */
  DBL
    Amplitude,  /* ��������� */
    PhaseShift, /* ����� �� ���� */
    ScaleShift; /* ������� ������� */
} MR3UNIT_BALL;

/* ������� ������������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT_BALL *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitInit( MR3UNIT_BALL *Uni, mr3ANIM *Ani )
{
  Uni->Pos = VecSet(rand() % 1000, rand() % 700, 0);
  Uni->Color = RGB(rand() % 256, rand() % 256, rand() % 256);
  Uni->PhaseShift = rand() % 3000;
  Uni->ScaleShift = 5 + 0.30 * rand() / RAND_MAX;
  Uni->Amplitude = 30 + 59.0 * rand() / RAND_MAX;
} /* End of 'MR3_AnimUnitInit' function */

/* ������� ��������������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT_BALL *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitClose( MR3UNIT_BALL *Uni, mr3ANIM *Ani )
{
} /* End of 'MR3_AnimUnitClose' function */

/* ������� ���������� ����������� ���������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT_BALL *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitResponse( MR3UNIT_BALL *Uni, mr3ANIM *Ani )
{

} /* End of 'MR3_AnimUnitResponse' function */

/* ������� ���������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT_BALL *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitRender( MR3UNIT_BALL *Uni, mr3ANIM *Ani )
{
  DBL shift = Uni->Amplitude * fabs(sin(Uni->ScaleShift * (DBL)clock() / CLOCKS_PER_SEC + Uni->PhaseShift));

  SetDCBrushColor(Ani->hDC, Uni->Color);
  Ellipse(Ani->hDC, Uni->Pos.X - 5, Uni->Pos.Y - 5 - shift, Uni->Pos.X + 5, Uni->Pos.Y + 5 - shift);
} /* End of 'MR3_AnimUnitRender' function */

/* ������� �������� ������� �������� "���".
 * ���������: ���.
 * ������������ ��������:
 *   (MR3UNIT *) ��������� �� ��������� ������ ��������.
 */
mr3UNIT * MR3_UnitBallCreate( VOID )
{
  MR3UNIT_BALL *Uni;

  if ((Uni = (VOID *)MR3_AnimUnitCreate(sizeof(MR3UNIT_BALL))) == NULL)
    return NULL;
  /* ��������� ���� */
  Uni->Init = (VOID *)MR3_AnimUnitInit;
  Uni->Close = (VOID *)MR3_AnimUnitClose;
  Uni->Response = (VOID *)MR3_AnimUnitResponse;
  Uni->Render = (VOID *)MR3_AnimUnitRender;
  return (mr3UNIT *)Uni;
} /* End of 'MR3_UnitBallCreate' function */

/* END OF 'UBALL.C' FILE */