#pragma once
/* Copyright 2018 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */
#include "BaseEngine.h"

namespace PdfEngine {

bool IsSupportedFile(const WCHAR* fileName, bool sniff = false);
BaseEngine* CreateFromFile(const WCHAR* fileName, PasswordUI* pwdUI = nullptr);
BaseEngine* CreateFromStream(IStream* stream, PasswordUI* pwdUI = nullptr);
double SetPdfStrokeLineMinWeight(double min_weight);
double GetPdfStrokeLineMinWeight();
double SetPdfStrokeLineMinFlatness(double min_flatness);
double GetPdfStrokeLineMinFlatness();
double SetPdfStrokeLineMaxWeight(double max_weight);
double GetPdfStrokeLineMaxWeight();
double SetPdfStrokeLineMaxFlatness(double max_flatness);
double GetPdfStrokeLineMaxFlatness();

} // namespace PdfEngine

namespace XpsEngine {

bool IsSupportedFile(const WCHAR* fileName, bool sniff = false);
BaseEngine* CreateFromFile(const WCHAR* fileName);
BaseEngine* CreateFromStream(IStream* stream);

} // namespace XpsEngine
