#pragma once
#include "f_ent.h"

void WritePointToClip(f3dPoint point);
BOOL ReadClipPoint(f3dPoint &point);
void ClearClipboard();
void WriteUCSToClip(UCS_STRU &ucs,FILE *fp=NULL);
BOOL ReadClipUCS(UCS_STRU &ucs,FILE *fp=NULL);
void WritePlaneToClip(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp=NULL);
BOOL ReadClipPlane(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp=NULL);