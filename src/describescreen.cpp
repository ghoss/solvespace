//-----------------------------------------------------------------------------
// The screens when an entity is selected, that show some description of it--
// endpoints of the lines, diameter of the circle, etc.
//
// Copyright 2008-2013 Jonathan Westhues.
//-----------------------------------------------------------------------------
#include "solvespace.h"

void TextWindow::ScreenUnselectAll(int link, uint32_t v) {
    GraphicsWindow::MenuEdit(Command::UNSELECT_ALL);
}

void TextWindow::ScreenEditTtfText(int link, uint32_t v) {
    hRequest hr = { v };
    Request *r = SK.GetRequest(hr);

    SS.TW.ShowEditControl(10, r->str);
    SS.TW.edit.meaning = Edit::TTF_TEXT;
    SS.TW.edit.request = hr;
}

void TextWindow::ScreenSetTtfFont(int link, uint32_t v) {
    int i = (int)v;
    if(i < 0) return;
    if(i >= SS.fonts.l.n) return;

    SS.GW.GroupSelection();
    auto const &gs = SS.GW.gs;
    if(gs.entities != 1 || gs.n != 1) return;

    Entity *e = SK.entity.FindByIdNoOops(gs.entity[0]);
    if(!e || e->type != Entity::Type::TTF_TEXT || !e->h.isFromRequest()) return;

    Request *r = SK.request.FindByIdNoOops(e->h.request());
    if(!r) return;

    SS.UndoRemember();
    r->font = SS.fonts.l.elem[i].FontFileBaseName();
    SS.MarkGroupDirty(r->group);
    SS.ScheduleShowTW();
}

void TextWindow::ScreenConstraintShowAsRadius(int link, uint32_t v) {
    hConstraint hc = { v };
    Constraint *c = SK.GetConstraint(hc);

    SS.UndoRemember();
    c->other = !c->other;

    SS.ScheduleShowTW();
}

void TextWindow::DescribeSelection() {
    Printf(false, "");

    auto const &gs = SS.GW.gs;
    if(gs.n == 1 && (gs.points == 1 || gs.entities == 1)) {
        Entity *e = SK.GetEntity(gs.points == 1 ? gs.point[0] : gs.entity[0]);
        Vector p;

#define COSTR(p) \
    SS.MmToString((p).x).c_str(), \
    SS.MmToString((p).y).c_str(), \
    SS.MmToString((p).z).c_str()
#define PT_AS_STR "(%Fi%s%E, %Fi%s%E, %Fi%s%E)"
#define PT_AS_NUM "(%Fi%#%E, %Fi%#%E, %Fi%#%E)"
        switch(e->type) {
            case Entity::Type::POINT_IN_3D:
            case Entity::Type::POINT_IN_2D:
            case Entity::Type::POINT_N_TRANS:
            case Entity::Type::POINT_N_ROT_TRANS:
            case Entity::Type::POINT_N_COPY:
            case Entity::Type::POINT_N_ROT_AA:
                p = e->PointGetNum();
                Printf(false, "%Ft%s%E %s " PT_AS_STR, _("POINT"), _("at"), COSTR(p));
                break;

            case Entity::Type::NORMAL_IN_3D:
            case Entity::Type::NORMAL_IN_2D:
            case Entity::Type::NORMAL_N_COPY:
            case Entity::Type::NORMAL_N_ROT:
            case Entity::Type::NORMAL_N_ROT_AA: {
                Quaternion q = e->NormalGetNum();
                p = q.RotationN();
                Printf(false, "%Ft%s%E", _("NORMAL / COORDINATE SYSTEM"));
                Printf(true, "%s:", _("BASIS"));
                Printf(false,  "n = " PT_AS_NUM, _("basis"), CO(p));
                p = q.RotationU();
                Printf(false, "u = " PT_AS_NUM, CO(p));
                p = q.RotationV();
                Printf(false, "v = " PT_AS_NUM, CO(p));
                break;
            }
            case Entity::Type::WORKPLANE: {
                p = SK.GetEntity(e->point[0])->PointGetNum();
                Printf(false, "%Ft%s%E", _("WORKPLANE"));
                Printf(true, "%S = " PT_AS_STR, _("origin"), -10, COSTR(p));
                Quaternion q = e->Normal()->NormalGetNum();
                p = q.RotationN();
                Printf(false, "%S = " PT_AS_NUM, _("normal"), -10, CO(p));
                break;
            }
            case Entity::Type::LINE_SEGMENT: {
                Vector p0 = SK.GetEntity(e->point[0])->PointGetNum();
                p = p0;
                Printf(false, "%Ft%s%E", _("LINE SEGMENT"));
                Printf(true,  "%S = " PT_AS_STR, _("pointA"), -10, COSTR(p));
                Vector p1 = SK.GetEntity(e->point[1])->PointGetNum();
                p = p1;
                Printf(false, "%S = " PT_AS_STR, _("pointB"), -10, COSTR(p));
                Printf(true,  "%S = %Fi%s%E", _("len"), -10,
                    SS.MmToString((p1.Minus(p0).Magnitude())).c_str());
                break;
            }
            case Entity::Type::CUBIC_PERIODIC:
            case Entity::Type::CUBIC:
                int pts;
                if(e->type == Entity::Type::CUBIC_PERIODIC) {
                    Printf(false, "%Ft%s%E", _("PERIODIC C2 CUBIC SPLINE"));
                    pts = (3 + e->extraPoints);
                } else if(e->extraPoints > 0) {
                    Printf(false, "%Ft%s%E", _("INTERPOLATING C2 CUBIC SPLINE"));
                    pts = (4 + e->extraPoints);
                } else {
                    Printf(false, "%Ft%s%E", _("CUBIC BEZIER CURVE"));
                    pts = 4;
                }
                for(int i = 0; i < pts; i++) {
                    p = SK.GetEntity(e->point[i])->PointGetNum();
                    Printf((i==0), "p%d = " PT_AS_STR, i, COSTR(p));
                }
                break;

            case Entity::Type::ARC_OF_CIRCLE: {
                Printf(false, "%Ft%s%E", _("ARC OF A CIRCLE"));
                p = SK.GetEntity(e->point[0])->PointGetNum();
                Printf(true,  "%S = " PT_AS_STR, _("center"), -15, COSTR(p));
                p = SK.GetEntity(e->point[1])->PointGetNum();
                Printf(true,  "%S = " PT_AS_STR, _("endpoints"), -15, COSTR(p));
                p = SK.GetEntity(e->point[2])->PointGetNum();
                Printf(false, "%S   " PT_AS_STR, "", -15, COSTR(p));
                double r = e->CircleGetRadiusNum();
                Printf(true, "%S =  %Fi%s", _("diameter"), -15, SS.MmToString(r*2).c_str());
                Printf(false, "%S =  %Fi%s", _("radius"), -15, SS.MmToString(r).c_str());
                double thetas, thetaf, dtheta;
                e->ArcGetAngles(&thetas, &thetaf, &dtheta);
                Printf(false, "%S =  %Fi%s", _("arc len"), -15,
                	SS.MmToString(dtheta*r).c_str());
                break;
            }
            case Entity::Type::CIRCLE: {
                Printf(false, "%Ft%s%E", _("CIRCLE"));
                p = SK.GetEntity(e->point[0])->PointGetNum();
                Printf(true,  "%S = " PT_AS_STR, _("center"), -15, COSTR(p));
                double r = e->CircleGetRadiusNum();
                Printf(true,  "%S =  %Fi%s", _("diameter"), -15, SS.MmToString(r*2).c_str());
                Printf(false, "%S =  %Fi%s", _("radius"), -15, SS.MmToString(r).c_str());
                break;
            }
            case Entity::Type::FACE_NORMAL_PT:
            case Entity::Type::FACE_XPROD:
            case Entity::Type::FACE_N_ROT_TRANS:
            case Entity::Type::FACE_N_ROT_AA:
            case Entity::Type::FACE_N_TRANS:
                Printf(false, "%Ft%s%E", _("PLANE FACE"));
                p = e->FaceGetNormalNum();
                Printf(true,  "%S = " PT_AS_NUM, _("normal"), -10, CO(p));
                p = e->FaceGetPointNum();
                Printf(false, "%S = " PT_AS_STR, _("thru"), -10, COSTR(p));
                break;

            case Entity::Type::TTF_TEXT: {
                Printf(false, "%Ft%s%E", _("TRUETYPE FONT TEXT"));
                Printf(true, "%S = '%Fi%s%E'", _("font"), -10, e->font.c_str());
                if(e->h.isFromRequest()) {
                    Printf(false, "%S = '%Fi%s%E' %Fl%Ll%f%D[%s]%E", _("text"), -10,
                    	e->str.c_str(), &ScreenEditTtfText, e->h.request().v, _("change"));
                    Printf(true, "%s:", _("select new font"));
                    SS.fonts.LoadAll();
                    int i;
                    for(i = 0; i < SS.fonts.l.n; i++) {
                        TtfFont *tf = &(SS.fonts.l.elem[i]);
                        if(e->font == tf->FontFileBaseName()) {
                            Printf(false, "%Bp%s",
                                (i & 1) ? 'd' : 'a',
                                tf->name.c_str());
                        } else {
                            Printf(false, "%Bp%f%D%Fl%Ll%s%E%Bp",
                                (i & 1) ? 'd' : 'a',
                                &ScreenSetTtfFont, i,
                                tf->name.c_str(),
                                (i & 1) ? 'd' : 'a');
                        }
                    }
                } else {
                    Printf(false, "%S = '%Fi%s%E'", _("text"), -10, e->str.c_str());
                }
                break;
            }
            case Entity::Type::IMAGE: {
                Printf(false, "%Ft%s%E", _("IMAGE"));
                Platform::Path relativePath = e->file.RelativeTo(SS.saveFile.Parent());
                if(relativePath.IsEmpty()) {
                    Printf(true, "%s = '%Fi%s%E'", _("file"), e->file.raw.c_str());
                } else {
                    Printf(true, "%s = '%Fi%s%E'", _("file"), relativePath.raw.c_str());
                }
                break;
            }

            default:
                Printf(true, "%Ft?? %s%E", _("ENTITY"));
                break;
        }

        Group *g = SK.GetGroup(e->group);
        Printf(false, "");
        Printf(false, "%Ft%S%E", _("IN GROUP"), -15, g->DescriptionString().c_str());
        if(e->workplane.v == Entity::FREE_IN_3D.v) {
            Printf(false, "%Ft%s%E", _("NOT LOCKED IN WORKPLANE"));
        } else {
            Entity *w = SK.GetEntity(e->workplane);
            Printf(false, "%Ft%S%E %s", _("IN WORKPLANE"), -15,
            	w->DescriptionString().c_str());
        }
        if(e->style.v) {
            Style *s = Style::Get(e->style);
            Printf(false, "%Ft%S%E %s", _("IN STYLE"), -15, s->DescriptionString().c_str());
        } else {
            Printf(false, "%Ft%S%E %s", _("IN STYLE"), -15, _("none"));
        }
        if(e->construction) {
            Printf(false, "%Ft%s", _("CONSTRUCTION"));
        }

        std::vector<hConstraint> lhc = {};
        for(const Constraint &c : SK.constraint) {
            if(!(c.ptA.v == e->h.v ||
                 c.ptB.v == e->h.v ||
                 c.entityA.v == e->h.v ||
                 c.entityB.v == e->h.v ||
                 c.entityC.v == e->h.v ||
                 c.entityD.v == e->h.v)) continue;
            lhc.push_back(c.h);
        }

        if(!lhc.empty()) {
            Printf(true, "%Ft%s:%E", _("CONSTRAINED BY"));

            int a = 0;
            for(hConstraint hc : lhc) {
                Constraint *c = SK.GetConstraint(hc);
                std::string s = c->DescriptionString();
                Printf(false, "%Bp   %Fl%Ll%D%f%h%s%E %s",
                    (a & 1) ? 'd' : 'a',
                    c->h.v, (&TextWindow::ScreenSelectConstraint),
                    (&TextWindow::ScreenHoverConstraint), s.c_str(),
                    c->reference ? "(ref)" : "");
                a++;
            }
        }
    } else if(gs.n == 2 && gs.points == 2) {
        Printf(false, "%Ft%s", _("TWO POINTS"));
        Vector p0 = SK.GetEntity(gs.point[0])->PointGetNum();
        Printf(true,  "   at " PT_AS_STR, COSTR(p0));
        Vector p1 = SK.GetEntity(gs.point[1])->PointGetNum();
        Printf(false, "      " PT_AS_STR, COSTR(p1));
        double d = (p1.Minus(p0)).Magnitude();
        Printf(true, "  d = %Fi%s", SS.MmToString(d).c_str());
    } else if(gs.n == 2 && gs.points == 1 && gs.circlesOrArcs == 1) {
        Entity *ec = SK.GetEntity(gs.entity[0]);
        if(ec->type == Entity::Type::CIRCLE) {
            Printf(false, "%Ft%s", _("POINT AND A CIRCLE"));
        } else if(ec->type == Entity::Type::ARC_OF_CIRCLE) {
            Printf(false, "%Ft%s", _("POINT AND AN ARC"));
        } else ssassert(false, "Unexpected entity type");
        Vector p = SK.GetEntity(gs.point[0])->PointGetNum();
        Printf(true,  "        %s " PT_AS_STR, _("pt at"), COSTR(p));
        Vector c = SK.GetEntity(ec->point[0])->PointGetNum();
        Printf(true,  "     %s = " PT_AS_STR, _("center"), COSTR(c));
        double r = ec->CircleGetRadiusNum();
        Printf(false, "   %s =  %Fi%s", _("diameter"), SS.MmToString(r*2).c_str());
        Printf(false, "     %s =  %Fi%s", _("radius"), SS.MmToString(r).c_str());
        double d = (p.Minus(c)).Magnitude() - r;
        Printf(true,  "   %s = %Fi%s", _("distance"), SS.MmToString(d).c_str());
    } else if(gs.n == 2 && gs.faces == 1 && gs.points == 1) {
        Printf(false, "%Ft", _("A POINT AND A PLANE FACE"));
        Vector pt = SK.GetEntity(gs.point[0])->PointGetNum();
        Printf(true,  "        %s = " PT_AS_STR, _("point"), COSTR(pt));
        Vector n = SK.GetEntity(gs.face[0])->FaceGetNormalNum();
        Printf(true,  " %s = " PT_AS_NUM, _("plane normal"), CO(n));
        Vector pl = SK.GetEntity(gs.face[0])->FaceGetPointNum();
        Printf(false, "   %s = " PT_AS_STR, _("plane thru"), COSTR(pl));
        double dd = n.Dot(pl) - n.Dot(pt);
        Printf(true,  "     %s = %Fi%s", _("distance"), SS.MmToString(dd).c_str());
    } else if(gs.n == 3 && gs.points == 2 && gs.vectors == 1) {
        Printf(false, "%Ft%s", _("TWO POINTS AND A VECTOR"));
        Vector p0 = SK.GetEntity(gs.point[0])->PointGetNum();
        Printf(true,  "  %s = " PT_AS_STR, _("pointA"), COSTR(p0));
        Vector p1 = SK.GetEntity(gs.point[1])->PointGetNum();
        Printf(false, "  %s = " PT_AS_STR, _("pointB"), COSTR(p1));
        Vector v  = SK.GetEntity(gs.vector[0])->VectorGetNum();
        v = v.WithMagnitude(1);
        Printf(true,  "  %s = " PT_AS_NUM, _("vector"), CO(v));
        double d = (p1.Minus(p0)).Dot(v);
        Printf(true,  "  %s = %Fi%s", _("proj_d"), SS.MmToString(d).c_str());
    } else if(gs.n == 2 && gs.lineSegments == 1 && gs.points == 1) {
        Entity *ln = SK.GetEntity(gs.entity[0]);
        Vector lp0 = SK.GetEntity(ln->point[0])->PointGetNum(),
               lp1 = SK.GetEntity(ln->point[1])->PointGetNum();
        Printf(false, "%Ft%s%E", _("LINE SEGMENT AND POINT"));
        Printf(true,  "%S " PT_AS_STR, _("ln thru"), -15, COSTR(lp0));
        Printf(false, "%S " PT_AS_STR, "", -15, COSTR(lp1));
        Entity *p  = SK.GetEntity(gs.point[0]);
        Vector pp = p->PointGetNum();
        Printf(true,  "%S " PT_AS_STR, _("point"), -15, COSTR(pp));
        Printf(true,  "%s = %Fi%s%E", _("pt-ln distance"), 
            SS.MmToString(pp.DistanceToLine(lp0, lp1.Minus(lp0))).c_str());
        hEntity wrkpl = SS.GW.ActiveWorkplane();
        if(wrkpl.v != Entity::FREE_IN_3D.v &&
                !(p->workplane.v == wrkpl.v && ln->workplane.v == wrkpl.v)) {
            Vector ppw  = pp.ProjectInto(wrkpl);
            Vector lp0w = lp0.ProjectInto(wrkpl);
            Vector lp1w = lp1.ProjectInto(wrkpl);
            Printf(false, "    %s = %Fi%s%E (%s)",
            	_("or distance"),
                SS.MmToString(ppw.DistanceToLine(lp0w, lp1w.Minus(lp0w))).c_str(),
                _("in workplane"));
        }
    } else if(gs.n == 2 && gs.vectors == 2) {
        Printf(false, "%Ft%s", _("TWO VECTORS"));

        Vector v0 = SK.GetEntity(gs.entity[0])->VectorGetNum(),
               v1 = SK.GetEntity(gs.entity[1])->VectorGetNum();
        v0 = v0.WithMagnitude(1);
        v1 = v1.WithMagnitude(1);

        Printf(true,  "%s = " PT_AS_NUM, _("vectorA"), CO(v0));
        Printf(false, "%s = " PT_AS_NUM, _("vectorB"), CO(v1));

        double theta = acos(v0.Dot(v1));
        Printf(true,  "%S = %Fi%2%E %s", _("angle"), -15, theta*180/PI, _("degrees"));
        while(theta < PI/2) theta += PI;
        while(theta > PI/2) theta -= PI;
        Printf(false, "%S = %Fi%2%E (mod 180)", _("or angle"), -15, theta*180/PI);
    } else if(gs.n == 2 && gs.faces == 2) {
        Printf(false, "%Ft%s", _("TWO PLANE FACES"));

        Vector n0 = SK.GetEntity(gs.face[0])->FaceGetNormalNum();
        Printf(true,  " %s = " PT_AS_NUM, _("planeA normal"), CO(n0));
        Vector p0 = SK.GetEntity(gs.face[0])->FaceGetPointNum();
        Printf(false, "   %s = " PT_AS_STR, _("planeA thru"), COSTR(p0));

        Vector n1 = SK.GetEntity(gs.face[1])->FaceGetNormalNum();
        Printf(true,  " %s = " PT_AS_NUM, _("planeB normal"), CO(n1));
        Vector p1 = SK.GetEntity(gs.face[1])->FaceGetPointNum();
        Printf(false, "   %s = " PT_AS_STR, _("planeB thru"), COSTR(p1));

        double theta = acos(n0.Dot(n1));
        Printf(true,  "         %s = %Fi%2%E %s", _("angle"), theta*180/PI, _("degrees"));
        while(theta < PI/2) theta += PI;
        while(theta > PI/2) theta -= PI;
        Printf(false, "      %s = %Fi%2%E (mod 180)", _("or angle"), theta*180/PI);

        if(fabs(theta) < 0.01) {
            double d = (p1.Minus(p0)).Dot(n0);
            Printf(true,  "      %s = %Fi%s", _("distance"), SS.MmToString(d).c_str());
        }
    } else if(gs.n == 0 && gs.stylables > 0) {
        Printf(false, "%Ft%s:%E %s", _("SELECTED"), _("comment text"));
    } else if(gs.n == 0 && gs.constraints == 1) {
        Constraint *c = SK.GetConstraint(gs.constraint[0]);

        if(c->type == Constraint::Type::DIAMETER) {
            Printf(false, "%Ft%s", _("DIAMETER CONSTRAINT"));

            Printf(true, "  %Fd%f%D%Ll%s  %s",
                   &ScreenConstraintShowAsRadius, gs.constraint[0].v,
                   c->other ? CHECK_TRUE : CHECK_FALSE, _("show as radius"));
        } else {
            Printf(false, "%Ft%s:%E %s", _("SELECTED"),
            c->DescriptionString().c_str());
        }

        std::vector<hEntity> lhe = {};
        lhe.push_back(c->ptA);
        lhe.push_back(c->ptB);
        lhe.push_back(c->entityA);
        lhe.push_back(c->entityB);
        lhe.push_back(c->entityC);
        lhe.push_back(c->entityD);

        auto it = std::remove_if(lhe.begin(), lhe.end(),
            [](hEntity he) {
                return he.v == Entity::NO_ENTITY.v || !he.isFromRequest();
            });
        lhe.erase(it, lhe.end());

        if(!lhe.empty()) {
            Printf(true, "%Ft%s:%E", _("CONSTRAINS"));

            int a = 0;
            for(hEntity he : lhe) {
                Request *r = SK.GetRequest(he.request());
                std::string s = r->DescriptionString();
                Printf(false, "%Bp   %Fl%Ll%D%f%h%s%E",
                    (a & 1) ? 'd' : 'a',
                    r->h.v, (&TextWindow::ScreenSelectRequest),
                    &(TextWindow::ScreenHoverRequest), s.c_str());
                a++;
            }
        }
    } else {
        int n = SS.GW.selection.n;
        Printf(false, "%Ft%s:%E %d %s", _("SELECTED"), n, n == 1 ? _("item") : _("items"));
    }

    if(shown.screen == Screen::STYLE_INFO &&
       shown.style.v >= Style::FIRST_CUSTOM && gs.stylables > 0)
    {
        // If we are showing a screen for a particular style, then offer the
        // option to assign our selected entities to that style.
        Style *s = Style::Get(shown.style);
        Printf(true, "%Fl%D%f%Ll(%s %s)%E",
            shown.style.v,
            &ScreenAssignSelectionToStyle,
            _("assign to style"),
            s->DescriptionString().c_str());
    }
    // If any of the selected entities have an assigned style, then offer
    // the option to remove that style.
    bool styleAssigned = false;
    for(int i = 0; i < gs.entities; i++) {
        Entity *e = SK.GetEntity(gs.entity[i]);
        if(e->style.v != 0) {
            styleAssigned = true;
        }
    }
    for(int i = 0; i < gs.constraints; i++) {
        Constraint *c = SK.GetConstraint(gs.constraint[i]);
        if(c->type == Constraint::Type::COMMENT && c->disp.style.v != 0) {
            styleAssigned = true;
        }
    }
    if(styleAssigned) {
        Printf(true, "%Fl%D%f%Ll(%s)%E",
            0,
            &ScreenAssignSelectionToStyle,
            _("remove assigned style"));
    }

    Printf(true, "%Fl%f%Ll(%s)%E", &TextWindow::ScreenUnselectAll, _("unselect all"));
}

void TextWindow::GoToScreen(Screen screen) {
    shown.screen = screen;
}

