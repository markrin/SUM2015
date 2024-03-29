#include <string.h>

#include "anim.h"

/* ������� ��-��������� ������������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitInit( mr3UNIT *Uni, mr3ANIM *Ani )
{
} /* End of 'MR3_AnimUnitInit' function */

/* ������� ��-��������� ��������������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitClose( mr3UNIT *Uni, mr3ANIM *Ani )
{
} /* End of 'MR3_AnimUnitClose' function */

/* ������� ��-��������� ���������� ����������� ���������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitResponse( mr3UNIT *Uni, mr3ANIM *Ani )
{
} /* End of 'MR3_AnimUnitResponse' function */

/* ������� ��-��������� ���������� ������� ��������.
 * ���������:
 *   - ��������� �� "����" - ��� ������ ��������:
 *       MR3UNIT *Uni;
 *   - ��������� �� �������� ��������:
 *       mr3ANIM *Ani;
 * ������������ ��������: ���.
 */
static VOID MR3_AnimUnitRender( mr3UNIT *Uni, mr3ANIM *Ani )
{
} /* End of 'MR3_AnimUnitRender' function */

/* ������� �������� ������� ��������.
 * ���������:
 *   - ������ ��������� ������� ��������:
 *       INT Size;
 * ������������ ��������:
 *   (MR3UNIT *) ��������� �� ��������� ������ ��������.
 */
mr3UNIT * MR3_AnimUnitCreate( INT Size )
{
  mr3UNIT *Uni;

  if (Size < sizeof(mr3UNIT) || (Uni = malloc(Size)) == NULL)
    return NULL;
  memset(Uni, 0, Size);
  /* ��������� ���� ��-��������� */
  Uni->Size = Size;
  Uni->Init = MR3_AnimUnitInit;
  Uni->Close = MR3_AnimUnitClose;
  Uni->Response = MR3_AnimUnitResponse;
  Uni->Render = MR3_AnimUnitRender;
  return Uni;
} /* End of 'MR3_AnimUnitCreate' function */

/* END OF 'UNIT.C' FILE */
