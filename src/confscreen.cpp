//-----------------------------------------------------------------------------
// For the configuration screen, setup items that are not specific to the
// file being edited right now.
//
// Copyright 2008-2013 Jonathan Westhues.
//-----------------------------------------------------------------------------
#include "solvespace.h"

void TextWindow::ScreenChangeLightDirection(int link, uint32_t v) {
    SS.TW.ShowEditControl(8, ssprintf("%.2f, %.2f, %.2f", CO(SS.lightDir[v])));
    SS.TW.edit.meaning = Edit::LIGHT_DIRECTION;
    SS.TW.edit.i = v;
}

void TextWindow::ScreenChangeLightIntensity(int link, uint32_t v) {
    SS.TW.ShowEditControl(31, ssprintf("%.2f", SS.lightIntensity[v]));
    SS.TW.edit.meaning = Edit::LIGHT_INTENSITY;
    SS.TW.edit.i = v;
}

void TextWindow::ScreenChangeColor(int link, uint32_t v) {
    SS.TW.ShowEditControlWithColorPicker(13, SS.modelColor[v]);

    SS.TW.edit.meaning = Edit::COLOR;
    SS.TW.edit.i = v;
}

void TextWindow::ScreenChangeChordTolerance(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, ssprintf("%lg", SS.chordTol));
    SS.TW.edit.meaning = Edit::CHORD_TOLERANCE;
    SS.TW.edit.i = 0;
}

void TextWindow::ScreenChangeMaxSegments(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, ssprintf("%d", SS.maxSegments));
    SS.TW.edit.meaning = Edit::MAX_SEGMENTS;
    SS.TW.edit.i = 0;
}

void TextWindow::ScreenChangeExportChordTolerance(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, ssprintf("%lg", SS.exportChordTol));
    SS.TW.edit.meaning = Edit::CHORD_TOLERANCE;
    SS.TW.edit.i = 1;
}

void TextWindow::ScreenChangeExportMaxSegments(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, ssprintf("%d", SS.exportMaxSegments));
    SS.TW.edit.meaning = Edit::MAX_SEGMENTS;
    SS.TW.edit.i = 1;
}

void TextWindow::ScreenChangeCameraTangent(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, ssprintf("%.3f", 1000*SS.cameraTangent));
    SS.TW.edit.meaning = Edit::CAMERA_TANGENT;
}

void TextWindow::ScreenChangeGridSpacing(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, SS.MmToString(SS.gridSpacing));
    SS.TW.edit.meaning = Edit::GRID_SPACING;
}

void TextWindow::ScreenChangeDigitsAfterDecimal(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, ssprintf("%d", SS.UnitDigitsAfterDecimal()));
    SS.TW.edit.meaning = Edit::DIGITS_AFTER_DECIMAL;
}

void TextWindow::ScreenChangeExportScale(int link, uint32_t v) {
    SS.TW.ShowEditControl(5, ssprintf("%.3f", (double)SS.exportScale));
    SS.TW.edit.meaning = Edit::EXPORT_SCALE;
}

void TextWindow::ScreenChangeExportOffset(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, SS.MmToString(SS.exportOffset));
    SS.TW.edit.meaning = Edit::EXPORT_OFFSET;
}

void TextWindow::ScreenChangeFixExportColors(int link, uint32_t v) {
    SS.fixExportColors = !SS.fixExportColors;
}

void TextWindow::ScreenChangeBackFaces(int link, uint32_t v) {
    SS.drawBackFaces = !SS.drawBackFaces;
    SS.GW.persistentDirty = true;
    InvalidateGraphics();
}

void TextWindow::ScreenChangeShowContourAreas(int link, uint32_t v) {
    SS.showContourAreas = !SS.showContourAreas;
    InvalidateGraphics();
}

void TextWindow::ScreenChangeCheckClosedContour(int link, uint32_t v) {
    SS.checkClosedContour = !SS.checkClosedContour;
    InvalidateGraphics();
}

void TextWindow::ScreenChangeShadedTriangles(int link, uint32_t v) {
    SS.exportShadedTriangles = !SS.exportShadedTriangles;
    InvalidateGraphics();
}

void TextWindow::ScreenChangePwlCurves(int link, uint32_t v) {
    SS.exportPwlCurves = !SS.exportPwlCurves;
    InvalidateGraphics();
}

void TextWindow::ScreenChangeCanvasSizeAuto(int link, uint32_t v) {
    if(link == 't') {
        SS.exportCanvasSizeAuto = true;
    } else {
        SS.exportCanvasSizeAuto = false;
    }
    InvalidateGraphics();
}

void TextWindow::ScreenChangeCanvasSize(int link, uint32_t v) {
    double d;
    switch(v) {
        case  0: d = SS.exportMargin.left;      break;
        case  1: d = SS.exportMargin.right;     break;
        case  2: d = SS.exportMargin.bottom;    break;
        case  3: d = SS.exportMargin.top;       break;

        case 10: d = SS.exportCanvas.width;     break;
        case 11: d = SS.exportCanvas.height;    break;
        case 12: d = SS.exportCanvas.dx;        break;
        case 13: d = SS.exportCanvas.dy;        break;

        default: return;
    }
    int col = 13;
    if(v < 10) col = 11;
    SS.TW.ShowEditControl(col, SS.MmToString(d));
    SS.TW.edit.meaning = Edit::CANVAS_SIZE;
    SS.TW.edit.i = v;
}

void TextWindow::ScreenChangeGCodeParameter(int link, uint32_t v) {
    std::string buf;
    switch(link) {
        case 'd':
            SS.TW.edit.meaning = Edit::G_CODE_DEPTH;
            buf += SS.MmToString(SS.gCode.depth);
            break;

        case 's':
            SS.TW.edit.meaning = Edit::G_CODE_PASSES;
            buf += std::to_string(SS.gCode.passes);
            break;

        case 'F':
            SS.TW.edit.meaning = Edit::G_CODE_FEED;
            buf += SS.MmToString(SS.gCode.feed);
            break;

        case 'P':
            SS.TW.edit.meaning = Edit::G_CODE_PLUNGE_FEED;
            buf += SS.MmToString(SS.gCode.plungeFeed);
            break;
    }
    SS.TW.ShowEditControl(14, buf);
}

void TextWindow::ScreenChangeAutosaveInterval(int link, uint32_t v) {
    SS.TW.ShowEditControl(3, std::to_string(SS.autosaveInterval));
    SS.TW.edit.meaning = Edit::AUTOSAVE_INTERVAL;
}

void TextWindow::ShowConfiguration() {
    int i;
    Printf(true, "%Ft %s (r, g, b)", _("user color"));

    for(i = 0; i < SS.MODEL_COLORS; i++) {
        Printf(false, "%Bp   #%d:  %Bz  %Bp  (%@, %@, %@) %f%D%Ll%Fl[%s]%E",
            (i & 1) ? 'd' : 'a',
            i, &SS.modelColor[i],
            (i & 1) ? 'd' : 'a',
            SS.modelColor[i].redF(),
            SS.modelColor[i].greenF(),
            SS.modelColor[i].blueF(),
            &ScreenChangeColor, i,
            _("change"));
    }

    Printf(false, "");
    Printf(false, "%Ft %S%s",
    	_("light direction"), -30, _("intensity"));
    for(i = 0; i < 2; i++) {
        Printf(false, "%Bp   #%d  (%2,%2,%2)%Fl%D%f%Ll[%s]%E "
                      "%2 %Fl%D%f%Ll[%s]%E",
            (i & 1) ? 'd' : 'a', i,
            CO(SS.lightDir[i]), i, &ScreenChangeLightDirection, _("c"),
            SS.lightIntensity[i], i, &ScreenChangeLightIntensity, _("c"));
    }

    Printf(false, "");
    Printf(false, "%Ft %s%E", _("chord tolerance (in percents)"));
    Printf(false, "%Ba   %@ %% %Fl%Ll%f%D[%s]%E; %@ %s, %d %s",
        SS.chordTol,
        &ScreenChangeChordTolerance, 0, _("change"), SS.chordTolCalculated, _("mm"),
        SK.GetGroup(SS.GW.activeGroup)->displayMesh.l.n, _("triangles"));
    Printf(false, "%Ft %s%E", _("max piecewise linear segments"));
    Printf(false, "%Ba   %d %Fl%Ll%f[%s]%E",
        SS.maxSegments,
        &ScreenChangeMaxSegments,
        _("change"));

    Printf(false, "");
    Printf(false, "%Ft %s%E", _("export chord tolerance (in mm)"));
    Printf(false, "%Ba   %@ %Fl%Ll%f%D[%s]%E",
        SS.exportChordTol,
        &ScreenChangeExportChordTolerance, 0,
        _("change"));
    Printf(false, "%Ft %s%E", _("export max piecewise linear segments"));
    Printf(false, "%Ba   %d %Fl%Ll%f[%s]%E",
        SS.exportMaxSegments,
        &ScreenChangeExportMaxSegments,
        _("change"));

    Printf(false, "");
    Printf(false, "%Ft %s%E", _("perspective factor (0 for parallel)"));
    Printf(false, "%Ba   %# %Fl%Ll%f%D[%s]%E",
        SS.cameraTangent*1000,
        &ScreenChangeCameraTangent, 0,
        _("change"));

    Printf(false, "%Ft %s%E", _("snap grid spacing"));
    Printf(false, "%Ba   %s %Fl%Ll%f%D[%s]%E",
        SS.MmToString(SS.gridSpacing).c_str(),
        &ScreenChangeGridSpacing, 0, _("change"));
        
    Printf(false, "%Ft %s%E", _("digits after decimal point to show"));
    Printf(false, "%Ba   %d %Fl%Ll%f%D[%s]%E (%s '%s')",
        SS.UnitDigitsAfterDecimal(),
        &ScreenChangeDigitsAfterDecimal, 0, _("change"), _("e.g."),
        SS.MmToString(SS.StringToMm("1.23456789")).c_str());

    Printf(false, "");
    Printf(false, "%Ft %s%E", _("export scale factor (1:1=mm, 1:25.4=inch)"));
    Printf(false, "%Ba   1:%# %Fl%Ll%f%D[%s]%E",
        (double)SS.exportScale,
        &ScreenChangeExportScale, 0,
        _("change"));
    Printf(false, "%Ft %s%E", _("cutter radius offset (0=no offset)"));
    Printf(false, "%Ba   %s %Fl%Ll%f%D[%s]%E",
        SS.MmToString(SS.exportOffset).c_str(),
        &ScreenChangeExportOffset, 0,
        _("change"));

    Printf(false, "");
    Printf(false, "  %Fd%f%Ll%s  %s%E",
        &ScreenChangeShadedTriangles,
        SS.exportShadedTriangles ? CHECK_TRUE : CHECK_FALSE,
        _("export shaded 2d triangles"));
        
    if(fabs(SS.exportOffset) > LENGTH_EPS) {
        Printf(false, "  %Fd%s  %s%E ",
                      _("(since cutter radius is not zero)"), CHECK_TRUE,
                      _("curves as piecewise linear"));
    } else {
        Printf(false, "  %Fd%f%Ll%s  %s%E",
            &ScreenChangePwlCurves,
            SS.exportPwlCurves ? CHECK_TRUE : CHECK_FALSE,
            _("export curves as piecewise linear"));
    }
    Printf(false, "  %Fd%f%Ll%s  %s%E",
        &ScreenChangeFixExportColors,
        SS.fixExportColors ? CHECK_TRUE : CHECK_FALSE,
        _("fix white exported lines"));

    Printf(false, "");
    Printf(false, "%Ft %s%E", _("export canvas size"));
    Printf(false, "  %Fd%f%Fd%Lf%s %s%E  "
                  "%f%Fd%Lt%s %s%E",
        &ScreenChangeCanvasSizeAuto,
        !SS.exportCanvasSizeAuto ? RADIO_TRUE : RADIO_FALSE, _("fixed"),
        &ScreenChangeCanvasSizeAuto,
        SS.exportCanvasSizeAuto ? RADIO_TRUE : RADIO_FALSE, _("auto"));

    if(SS.exportCanvasSizeAuto) {
        Printf(false, "%Ft %s", _("by margins around exported geometry"));
        Printf(false, "%Ba%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("left:"), -10,
            SS.MmToString(SS.exportMargin.left).c_str(), 7, &ScreenChangeCanvasSize, 0,
            _("change"));
        Printf(false, "%Bd%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("right:"), -10,
            SS.MmToString(SS.exportMargin.right).c_str(), 7, &ScreenChangeCanvasSize, 1,
            _("change"));
        Printf(false, "%Ba%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("bottom:"), -10,
            SS.MmToString(SS.exportMargin.bottom).c_str(), 7, &ScreenChangeCanvasSize, 2,
            _("change"));
        Printf(false, "%Bd%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("top:"), -10,
            SS.MmToString(SS.exportMargin.top).c_str(), 7, &ScreenChangeCanvasSize, 3,
            _("change"));
    } else {
        Printf(false, "%Ft %s", _("by absolute dimensions and offsets"));
        Printf(false, "%Ba%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("width:"), -12,
            SS.MmToString(SS.exportCanvas.width).c_str(), 7, &ScreenChangeCanvasSize, 10,
            _("change"));
        Printf(false, "%Bd%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("height:"), -12,
            SS.MmToString(SS.exportCanvas.height).c_str(), 7, &ScreenChangeCanvasSize, 11,
            _("change"));
        Printf(false, "%Ba%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("offset x:"), -12,
            SS.MmToString(SS.exportCanvas.dx).c_str(), 7, &ScreenChangeCanvasSize, 12,
            _("change"));
        Printf(false, "%Bd%Ft   %S%Fd%S %Fl%Ll%f%D[%s]%E",
        	_("offset y:"), -12,
            SS.MmToString(SS.exportCanvas.dy).c_str(), 7, &ScreenChangeCanvasSize, 13,
            _("change"));
    }

    Printf(false, "");
    Printf(false, "%Ft %s", _("exported g code parameters"));
    Printf(false, "%Ba%Ft   %S%Fd%S %Fl%Ld%f[%s]%E",
    	_("depth:"), -12,
        SS.MmToString(SS.gCode.depth).c_str(), 7, &ScreenChangeGCodeParameter,
        _("change"));
    Printf(false, "%Bd%Ft   %S%Fd%S %Fl%Ls%f[%s]%E",
    	_("passes:"), -12,
        SS.MmToString(SS.gCode.passes).c_str(), 7, &ScreenChangeGCodeParameter,
        _("change"));
    Printf(false, "%Ba%Ft   %S%Fd%S %Fl%LF%f[%s]%E",
    	_("feed:"), -12,
        SS.MmToString(SS.gCode.feed).c_str(), 7, &ScreenChangeGCodeParameter,
        _("change"));
    Printf(false, "%Bd%Ft   %S%Fd%S %Fl%LP%f[%s]%E",
    	_("plunge fd:"), -12,
        SS.MmToString(SS.gCode.plungeFeed).c_str(), 7, &ScreenChangeGCodeParameter,
        _("change"));

    Printf(false, "");
    Printf(false, "  %Fd%f%Ll%s  %s%E",
        &ScreenChangeBackFaces,
        SS.drawBackFaces ? CHECK_TRUE : CHECK_FALSE,
        _("draw triangle back faces in red"));
    Printf(false, "  %Fd%f%Ll%s  %s%E",
        &ScreenChangeCheckClosedContour,
        SS.checkClosedContour ? CHECK_TRUE : CHECK_FALSE,
        _("check sketch for closed contour"));
    Printf(false, "  %Fd%f%Ll%s  %s%E",
        &ScreenChangeShowContourAreas,
        SS.showContourAreas ? CHECK_TRUE : CHECK_FALSE,
        _("show areas of closed contours"));

    Printf(false, "");
    Printf(false, "%Ft %s%E", _("autosave interval (in minutes)"));
    Printf(false, "%Ba   %d %Fl%Ll%f[%s]%E",
        SS.autosaveInterval, &ScreenChangeAutosaveInterval, _("change"));

    if(canvas) {
        const char *gl_vendor, *gl_renderer, *gl_version;
        canvas->GetIdent(&gl_vendor, &gl_renderer, &gl_version);
        Printf(false, "");
        Printf(false, " %Ftgl %S %E%s", _("vendor"), -15, gl_vendor);
        Printf(false, " %Ft   %S %E%s", _("renderer"), -15, gl_renderer);
        Printf(false, " %Ft   %S %E%s", _("version"), -15, gl_version);
    }
}

bool TextWindow::EditControlDoneForConfiguration(const char *s) {
    switch(edit.meaning) {
        case Edit::LIGHT_INTENSITY:
            SS.lightIntensity[edit.i] = min(1.0, max(0.0, atof(s)));
            InvalidateGraphics();
            break;

        case Edit::LIGHT_DIRECTION: {
            double x, y, z;
            if(sscanf(s, "%lf, %lf, %lf", &x, &y, &z)==3) {
                SS.lightDir[edit.i] = Vector::From(x, y, z);
            } else {
                Error(_("Bad format: specify coordinates as x, y, z"));
            }
            InvalidateGraphics();
            break;
        }
        case Edit::COLOR: {
            Vector rgb;
            if(sscanf(s, "%lf, %lf, %lf", &rgb.x, &rgb.y, &rgb.z)==3) {
                rgb = rgb.ClampWithin(0, 1);
                SS.modelColor[edit.i] = RGBf(rgb.x, rgb.y, rgb.z);
            } else {
                Error(_("Bad format: specify color as r, g, b"));
            }
            break;
        }
        case Edit::CHORD_TOLERANCE: {
            if(edit.i == 0) {
                SS.chordTol = max(0.0, atof(s));
                SS.GenerateAll(SolveSpaceUI::Generate::ALL);
            } else {
                SS.exportChordTol = max(0.0, atof(s));
            }
            break;
        }
        case Edit::MAX_SEGMENTS: {
            if(edit.i == 0) {
                SS.maxSegments = min(1000, max(7, atoi(s)));
                SS.GenerateAll(SolveSpaceUI::Generate::ALL);
            } else {
                SS.exportMaxSegments = min(1000, max(7, atoi(s)));
            }
            break;
        }
        case Edit::CAMERA_TANGENT: {
            SS.cameraTangent = (min(2.0, max(0.0, atof(s))))/1000.0;
            if(!SS.usePerspectiveProj) {
                Message(_("The perspective factor will have no effect until you "
                          "enable View -> Use Perspective Projection."));
            }
            InvalidateGraphics();
            break;
        }
        case Edit::GRID_SPACING: {
            SS.gridSpacing = (float)min(1e4, max(1e-3, SS.StringToMm(s)));
            InvalidateGraphics();
            break;
        }
        case Edit::DIGITS_AFTER_DECIMAL: {
            int v = atoi(s);
            if(v < 0 || v > 8) {
                Error(_("Specify between 0 and 8 digits after the decimal."));
            } else {
                SS.SetUnitDigitsAfterDecimal(v);
            }
            InvalidateGraphics();
            break;
        }
        case Edit::EXPORT_SCALE: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) {
                double ev = e->Eval();
                if(fabs(ev) < 0.001 || isnan(ev)) {
                    Error(_("Export scale must not be zero!"));
                } else {
                    SS.exportScale = (float)ev;
                }
            }
            break;
        }
        case Edit::EXPORT_OFFSET: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) {
                double ev = SS.ExprToMm(e);
                if(isnan(ev) || ev < 0) {
                    Error(_("Cutter radius offset must not be negative!"));
                } else {
                    SS.exportOffset = (float)ev;
                }
            }
            break;
        }
        case Edit::CANVAS_SIZE: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(!e) {
                break;
            }
            float d = (float)SS.ExprToMm(e);
            switch(edit.i) {
                case  0: SS.exportMargin.left   = d;    break;
                case  1: SS.exportMargin.right  = d;    break;
                case  2: SS.exportMargin.bottom = d;    break;
                case  3: SS.exportMargin.top    = d;    break;

                case 10: SS.exportCanvas.width  = d;    break;
                case 11: SS.exportCanvas.height = d;    break;
                case 12: SS.exportCanvas.dx     = d;    break;
                case 13: SS.exportCanvas.dy     = d;    break;
            }
            break;
        }
        case Edit::G_CODE_DEPTH: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) SS.gCode.depth = (float)SS.ExprToMm(e);
            break;
        }
        case Edit::G_CODE_PASSES: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) SS.gCode.passes = (int)(e->Eval());
            SS.gCode.passes = max(1, min(1000, SS.gCode.passes));
            break;
        }
        case Edit::G_CODE_FEED: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) SS.gCode.feed = (float)SS.ExprToMm(e);
            break;
        }
        case Edit::G_CODE_PLUNGE_FEED: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) SS.gCode.plungeFeed = (float)SS.ExprToMm(e);
            break;
        }
        case Edit::AUTOSAVE_INTERVAL: {
            int interval;
            if(sscanf(s, "%d", &interval)==1) {
                if(interval >= 1) {
                    SS.autosaveInterval = interval;
                    SetAutosaveTimerFor(interval);
                } else {
                    Error(_("Bad value: autosave interval should be positive"));
                }
            } else {
                Error(_("Bad format: specify interval in integral minutes"));
            }
            break;
        }

        default: return false;
    }
    return true;
}

