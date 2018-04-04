//-----------------------------------------------------------------------------
// The text-based browser window, used to view the structure of the model by
// groups and for other similar purposes.
//
// Copyright 2008-2013 Jonathan Westhues.
//-----------------------------------------------------------------------------
#include "solvespace.h"

//-----------------------------------------------------------------------------
// A navigation bar that always appears at the top of the window, with a
// link to bring us back home.
//-----------------------------------------------------------------------------
void TextWindow::ScreenHome(int link, uint32_t v) {
    SS.TW.GoToScreen(Screen::LIST_OF_GROUPS);
}
void TextWindow::ShowHeader(bool withNav) {
    ClearScreen();

    const char *header;
    std::string desc;
    if(SS.GW.LockedInWorkplane()) {
        header = _("in plane: ");
        desc = SK.GetEntity(SS.GW.ActiveWorkplane())->DescriptionString();
    } else {
        header = _("drawing / constraining in 3d");
        desc = "";
    }

    // Navigation buttons
    if(withNav) {
        Printf(false, " %Fl%Lh%f%s%E   %Ft%s%E%s",
            (&TextWindow::ScreenHome), _("home"), header, desc.c_str());
    } else {
        Printf(false, "        %Ft%s%E%s", header, desc.c_str());
    }

    // Leave space for the icons that are painted here.
    Printf(false, "");
    Printf(false, "");
}

//-----------------------------------------------------------------------------
// The screen that shows a list of every group in the sketch, with options
// to hide or show them, and to view them in detail. This is our home page.
//-----------------------------------------------------------------------------
void TextWindow::ScreenSelectGroup(int link, uint32_t v) {
    SS.TW.GoToScreen(Screen::GROUP_INFO);
    SS.TW.shown.group.v = v;
}
void TextWindow::ScreenToggleGroupShown(int link, uint32_t v) {
    hGroup hg = { v };
    Group *g = SK.GetGroup(hg);
    g->visible = !(g->visible);
    // If a group was just shown, then it might not have been generated
    // previously, so regenerate.
    SS.GenerateAll();
}
void TextWindow::ScreenShowGroupsSpecial(int link, uint32_t v) {
    bool state = link == 's';
    for(int i = 0; i < SK.groupOrder.n; i++) {
        Group *g = SK.GetGroup(SK.groupOrder.elem[i]);
        g->visible = state;
    }
    SS.GW.persistentDirty = true;
}
void TextWindow::ScreenActivateGroup(int link, uint32_t v) {
    SS.GW.activeGroup.v = v;
    SK.GetGroup(SS.GW.activeGroup)->Activate();
    SS.GW.ClearSuper();
}
void TextWindow::ReportHowGroupSolved(hGroup hg) {
    SS.GW.ClearSuper();
    SS.TW.GoToScreen(Screen::GROUP_SOLVE_INFO);
    SS.TW.shown.group.v = hg.v;
    SS.ScheduleShowTW();
}
void TextWindow::ScreenHowGroupSolved(int link, uint32_t v) {
    if(SS.GW.activeGroup.v != v) {
        ScreenActivateGroup(link, v);
    }
    SS.TW.GoToScreen(Screen::GROUP_SOLVE_INFO);
    SS.TW.shown.group.v = v;
}
void TextWindow::ScreenShowConfiguration(int link, uint32_t v) {
    SS.TW.GoToScreen(Screen::CONFIGURATION);
}
void TextWindow::ScreenShowEditView(int link, uint32_t v) {
    SS.TW.GoToScreen(Screen::EDIT_VIEW);
}
void TextWindow::ScreenGoToWebsite(int link, uint32_t v) {
    OpenWebsite("http://solvespace.com/txtlink");
}
void TextWindow::ShowListOfGroups() {
    const char *radioTrue  = " " RADIO_TRUE  " ",
               *radioFalse = " " RADIO_FALSE " ",
               *checkTrue  = " " CHECK_TRUE  " ",
               *checkFalse = " " CHECK_FALSE " ";

    Printf(true, "%Ft %S", _("active"), -10);
    Printf(false, "%Ft      %S%S%S%E", _("shown"), -10, _("ok"), -7, _("group-name"), -10);
    int i;
    bool afterActive = false;
    for(i = 0; i < SK.groupOrder.n; i++) {
        Group *g = SK.GetGroup(SK.groupOrder.elem[i]);
        std::string s = g->DescriptionString();
        bool active = (g->h.v == SS.GW.activeGroup.v);
        bool shown = g->visible;
        bool ok = g->IsSolvedOkay();
        bool ref = (g->h.v == Group::HGROUP_REFERENCES.v);
        Printf(false, "%Bp%Fd "
               "%Ft%s%Fb%D%f%Ll%s%E "
               "%Fb%s%D%f%Ll  %s%E  "
               "%Fp%D%f    %S%Ll%S%E  "
               "%Fl%Ll%D%f%s",
            // Alternate between light and dark backgrounds, for readability
                (i & 1) ? 'd' : 'a',
            // Link that activates the group
                ref ? "   " : "",
                g->h.v, (&TextWindow::ScreenActivateGroup),
                ref ? "" : (active ? radioTrue : radioFalse),
            // Link that hides or shows the group
                afterActive ? " - " : "",
                g->h.v, (&TextWindow::ScreenToggleGroupShown),
                afterActive ? "" : (shown ? checkTrue : checkFalse),
            // Link to the errors, if a problem occured while solving
            ok ? 's' : 'x', g->h.v, (&TextWindow::ScreenHowGroupSolved),
                ok ? _("ok") : "", ok ? -5 : 0,
                ok ? "" : _("NO"), ok ? 0 : -5,
            // Link to a screen that gives more details on the group
            g->h.v, (&TextWindow::ScreenSelectGroup), s.c_str());

        if(active) afterActive = true;
    }

    Printf(true,  "  %Fl%Ls%f%s%E / %Fl%Lh%f%s%E",
        &(TextWindow::ScreenShowGroupsSpecial), _("show all"),
        &(TextWindow::ScreenShowGroupsSpecial), _("hide all"));
    Printf(true,  "  %Fl%Ls%f%s%E /"
                   " %Fl%Ls%f%s%E /"
                   " %Fl%Ls%f%s%E",
        &(TextWindow::ScreenShowListOfStyles), _("line styles"),
        &(TextWindow::ScreenShowEditView), _("view"),
        &(TextWindow::ScreenShowConfiguration), _("configuration"));
}


//-----------------------------------------------------------------------------
// The screen that shows information about a specific group, and allows the
// user to edit various things about it.
//-----------------------------------------------------------------------------
void TextWindow::ScreenHoverConstraint(int link, uint32_t v) {
    if(!SS.GW.showConstraints) return;

    hConstraint hc = { v };
    SS.GW.hover.Clear();
    SS.GW.hover.constraint = hc;
    SS.GW.hover.emphasized = true;
}
void TextWindow::ScreenHoverRequest(int link, uint32_t v) {
    SS.GW.hover.Clear();
    hRequest hr = { v };
    SS.GW.hover.entity = hr.entity(0);
    SS.GW.hover.emphasized = true;
}
void TextWindow::ScreenSelectConstraint(int link, uint32_t v) {
    SS.GW.ClearSelection();
    GraphicsWindow::Selection sel = {};
    sel.constraint.v = v;
    SS.GW.selection.Add(&sel);
}
void TextWindow::ScreenSelectRequest(int link, uint32_t v) {
    SS.GW.ClearSelection();
    GraphicsWindow::Selection sel = {};
    hRequest hr = { v };
    sel.entity = hr.entity(0);
    SS.GW.selection.Add(&sel);
}

void TextWindow::ScreenChangeGroupOption(int link, uint32_t v) {
    SS.UndoRemember();
    Group *g = SK.GetGroup(SS.TW.shown.group);

    switch(link) {
        case 's': g->subtype = Group::Subtype::ONE_SIDED; break;
        case 'S': g->subtype = Group::Subtype::TWO_SIDED; break;

        case 'k': g->skipFirst = true; break;
        case 'K': g->skipFirst = false; break;

        case 'c':
            if(g->type == Group::Type::EXTRUDE) {
                // When an extrude group is first created, it's positioned for a union
                // extrusion. If no constraints were added, flip it when we switch between
                // union and difference modes to avoid manual work doing the smae.
                if(g->meshCombine != (Group::CombineAs)v && g->GetNumConstraints() == 0 &&
                        ((Group::CombineAs)v == Group::CombineAs::DIFFERENCE ||
                        g->meshCombine == Group::CombineAs::DIFFERENCE)) {
                    g->ExtrusionForceVectorTo(g->ExtrusionGetVector().Negated());
                }
            }
            g->meshCombine = (Group::CombineAs)v;
            break;

        case 'P': g->suppress = !(g->suppress); break;

        case 'r': g->relaxConstraints = !(g->relaxConstraints); break;

        case 'e': g->allowRedundant = !(g->allowRedundant); break;

        case 'v': g->visible = !(g->visible); break;

        case 'd': g->allDimsReference = !(g->allDimsReference); break;

        case 'f': g->forceToMesh = !(g->forceToMesh); break;
    }

    SS.MarkGroupDirty(g->h);
    SS.GW.ClearSuper();
}

void TextWindow::ScreenColor(int link, uint32_t v) {
    SS.UndoRemember();

    Group *g = SK.GetGroup(SS.TW.shown.group);
    SS.TW.ShowEditControlWithColorPicker(3, g->color);
    SS.TW.edit.meaning = Edit::GROUP_COLOR;
}
void TextWindow::ScreenOpacity(int link, uint32_t v) {
    Group *g = SK.GetGroup(SS.TW.shown.group);

    SS.TW.ShowEditControl(11, ssprintf("%.2f", g->color.alphaF()));
    SS.TW.edit.meaning = Edit::GROUP_OPACITY;
    SS.TW.edit.group.v = g->h.v;
}
void TextWindow::ScreenChangeExprA(int link, uint32_t v) {
    Group *g = SK.GetGroup(SS.TW.shown.group);

    SS.TW.ShowEditControl(10, ssprintf("%d", (int)g->valA));
    SS.TW.edit.meaning = Edit::TIMES_REPEATED;
    SS.TW.edit.group.v = v;
}
void TextWindow::ScreenChangeGroupName(int link, uint32_t v) {
    Group *g = SK.GetGroup(SS.TW.shown.group);
    SS.TW.ShowEditControl(12, g->DescriptionString().substr(5));
    SS.TW.edit.meaning = Edit::GROUP_NAME;
    SS.TW.edit.group.v = v;
}
void TextWindow::ScreenChangeGroupScale(int link, uint32_t v) {
    Group *g = SK.GetGroup(SS.TW.shown.group);

    SS.TW.ShowEditControl(13, ssprintf("%.3f", g->scale));
    SS.TW.edit.meaning = Edit::GROUP_SCALE;
    SS.TW.edit.group.v = v;
}
void TextWindow::ScreenDeleteGroup(int link, uint32_t v) {
    SS.UndoRemember();

    hGroup hg = SS.TW.shown.group;
    if(hg.v == SS.GW.activeGroup.v) {
        SS.GW.activeGroup = SK.GetGroup(SS.GW.activeGroup)->PreviousGroup()->h;
    }

    // Reset the text window, since we're displaying information about
    // the group that's about to get deleted.
    SS.TW.ClearSuper();

    // This is a major change, so let's re-solve everything.
    SK.group.RemoveById(hg);
    SS.GenerateAll(SolveSpaceUI::Generate::ALL);

    // Reset the graphics window. This will also recreate the default
    // group if it was removed.
    SS.GW.ClearSuper();
}
void TextWindow::ShowGroupInfo() {
    Group *g = SK.GetGroup(shown.group);
    const char *s = "???";

    if(shown.group.v == Group::HGROUP_REFERENCES.v) {
        Printf(true, "%Ft%s  %E%s", _("GROUP"), g->DescriptionString().c_str());
        goto list_items;
    } else {
        Printf(true, "%Ft%s  %E%s [%Fl%Ll%D%f%s%E/%Fl%Ll%D%f%s%E]",
            _("GROUP"),
            g->DescriptionString().c_str(),
            g->h.v, &TextWindow::ScreenChangeGroupName, _("rename"),
            g->h.v, &TextWindow::ScreenDeleteGroup, _("del"));
    }

    if(g->type == Group::Type::LATHE) {
        Printf(true, " %Ft%s", _("lathe plane sketch"));
    } else if(g->type == Group::Type::EXTRUDE || g->type == Group::Type::ROTATE ||
              g->type == Group::Type::TRANSLATE)
    {
        if(g->type == Group::Type::EXTRUDE) {
            s = _("extrude plane sketch");
        } else if(g->type == Group::Type::TRANSLATE) {
            s = _("translate original sketch");
        } else if(g->type == Group::Type::ROTATE) {
            s = _("rotate original sketch");
        }
        Printf(true, " %Ft%s%E", s);

        bool one = (g->subtype == Group::Subtype::ONE_SIDED);
        Printf(false,
            "%Ba   %f%Ls%Fd%s %s%E  "
                  "%f%LS%Fd%s %s%E",
            &TextWindow::ScreenChangeGroupOption,
            one ? RADIO_TRUE : RADIO_FALSE, _("one-sided"),
            &TextWindow::ScreenChangeGroupOption,
            !one ? RADIO_TRUE : RADIO_FALSE, _("two-sided"));

        if(g->type == Group::Type::ROTATE || g->type == Group::Type::TRANSLATE) {
            if(g->subtype == Group::Subtype::ONE_SIDED) {
                bool skip = g->skipFirst;
                Printf(false,
                   "%Bd   %Ft%s  %f%LK%Fd%s %s%E  "
                         "%f%Lk%Fd%s %s%E",
                	_("start"),
                    &ScreenChangeGroupOption,
                    !skip ? RADIO_TRUE : RADIO_FALSE, _("with original"),
                    &ScreenChangeGroupOption,
                    skip ? RADIO_TRUE : RADIO_FALSE, _("with copy #1"));
            }

            int times = (int)(g->valA);
            Printf(false, "%Bp   %Ftrepeat%E %d time%s %Fl%Ll%D%f[%s]%E",
                (g->subtype == Group::Subtype::ONE_SIDED) ? 'a' : 'd',
                times, times == 1 ? "" : "s",
                g->h.v, &TextWindow::ScreenChangeExprA, _("change"));
        }
    } else if(g->type == Group::Type::LINKED) {
        Printf(true, " %Ft%s%E");
        Platform::Path relativePath = g->linkFile.RelativeTo(SS.saveFile.Parent());
        if(relativePath.IsEmpty()) {
            Printf(false, "%Ba   '%s'", g->linkFile.raw.c_str());
        } else {
            Printf(false, "%Ba   '%s'", relativePath.raw.c_str());
        }
        Printf(false, "%Bd   %Ft%s%E %# %Fl%Ll%f%D[%s]%E",
        	_("scaled by"),
            g->scale,
            &TextWindow::ScreenChangeGroupScale, g->h.v, _("change"));
    } else if(g->type == Group::Type::DRAWING_3D) {
        Printf(true, " %Ft%s%E", _("sketch in 3d"));
    } else if(g->type == Group::Type::DRAWING_WORKPLANE) {
        Printf(true, " %Ft%s%E", _("sketch in new workplane"));
    } else {
        Printf(true, "???");
    }
    Printf(false, "");

    if(g->type == Group::Type::EXTRUDE ||
       g->type == Group::Type::LATHE ||
       g->type == Group::Type::LINKED)
    {
        bool un   = (g->meshCombine == Group::CombineAs::UNION);
        bool diff = (g->meshCombine == Group::CombineAs::DIFFERENCE);
        bool asy  = (g->meshCombine == Group::CombineAs::ASSEMBLE);

        Printf(false, " %Ft%s", _("solid model as"));
        Printf(false, "%Ba   %f%D%Lc%Fd%s %s%E  "
                             "%f%D%Lc%Fd%s %s%E  "
                             "%f%D%Lc%Fd%s %s%E  ",
            &TextWindow::ScreenChangeGroupOption,
            Group::CombineAs::UNION,
            un ? RADIO_TRUE : RADIO_FALSE, _("union"),
            &TextWindow::ScreenChangeGroupOption,
            Group::CombineAs::DIFFERENCE,
            diff ? RADIO_TRUE : RADIO_FALSE, _("difference"),
            &TextWindow::ScreenChangeGroupOption,
            Group::CombineAs::ASSEMBLE,
            (asy ? RADIO_TRUE : RADIO_FALSE), _("assemble"));

        if(g->type == Group::Type::EXTRUDE ||
           g->type == Group::Type::LATHE)
        {
            Printf(false,
                "%Bd   %Ftcolor   %E%Bz  %Bd (%@, %@, %@) %f%D%Lf%Fl[change]%E",
                &g->color,
                g->color.redF(), g->color.greenF(), g->color.blueF(),
                ScreenColor, top[rows-1] + 2);
            Printf(false, "%Bd   %Ftopacity%E %@ %f%Lf%Fl[change]%E",
                g->color.alphaF(),
                &TextWindow::ScreenOpacity);
        }

        if(g->type == Group::Type::EXTRUDE ||
           g->type == Group::Type::LATHE ||
           g->type == Group::Type::LINKED) {
            Printf(false, "   %Fd%f%LP%s  %s",
                &TextWindow::ScreenChangeGroupOption,
                g->suppress ? CHECK_TRUE : CHECK_FALSE,
                _("suppress this group's solid model"));
        }

        Printf(false, "");
    }

    Printf(false, " %f%Lv%Fd%s  %s",
        &TextWindow::ScreenChangeGroupOption,
        g->visible ? CHECK_TRUE : CHECK_FALSE,
        _("show entities from this group"));

    if(!g->IsForcedToMeshBySource()) {
        Printf(false, " %f%Lf%Fd%s  %s",
            &TextWindow::ScreenChangeGroupOption,
            g->forceToMesh ? CHECK_TRUE : CHECK_FALSE,
            _("force NURBS surfaces to triangle mesh"));
    } else {
        Printf(false, " (%s)", _("model already forced to triangle mesh"));
    }

    Printf(true, " %f%Lr%Fd%s  %s",
        &TextWindow::ScreenChangeGroupOption,
        g->relaxConstraints ? CHECK_TRUE : CHECK_FALSE,
        _("relax constraints and dimensions"));

    Printf(false, " %f%Le%Fd%s  %s",
        &TextWindow::ScreenChangeGroupOption,
        g->allowRedundant ? CHECK_TRUE : CHECK_FALSE,
        _("allow redundant constraints"));

    Printf(false, " %f%Ld%Fd%s  %s",
        &TextWindow::ScreenChangeGroupOption,
        g->allDimsReference ? CHECK_TRUE : CHECK_FALSE,
        _("treat all dimensions as reference"));

    if(g->booleanFailed) {
        Printf(false, "");
        Printf(false, "%s", _("The Boolean operation failed. It may be "));
        Printf(false, "%s", _("possible to fix the problem by choosing "));
        Printf(false, "%s", _("'force NURBS surfaces to triangle mesh'."));
    }

list_items:
    Printf(false, "");
    Printf(false, "%Ft %s", _("requests in group"));

    int i, a = 0;
    for(i = 0; i < SK.request.n; i++) {
        Request *r = &(SK.request.elem[i]);

        if(r->group.v == shown.group.v) {
            std::string s = r->DescriptionString();
            Printf(false, "%Bp   %Fl%Ll%D%f%h%s%E",
                (a & 1) ? 'd' : 'a',
                r->h.v, (&TextWindow::ScreenSelectRequest),
                &(TextWindow::ScreenHoverRequest), s.c_str());
            a++;
        }
    }
    if(a == 0) Printf(false, "%Ba   %s", _("(none)"));

    a = 0;
    Printf(false, "");
    Printf(false, "%Ft %s (%d %s)", _("constraints in group"), g->solved.dof, _("DOF"));
    for(i = 0; i < SK.constraint.n; i++) {
        Constraint *c = &(SK.constraint.elem[i]);

        if(c->group.v == shown.group.v) {
            std::string s = c->DescriptionString();
            Printf(false, "%Bp   %Fl%Ll%D%f%h%s%E %s",
                (a & 1) ? 'd' : 'a',
                c->h.v, (&TextWindow::ScreenSelectConstraint),
                (&TextWindow::ScreenHoverConstraint), s.c_str(),
                c->reference ? _("(ref)") : "");
            a++;
        }
    }
    if(a == 0) Printf(false, "%Ba   %s", _("(none)"));
}

//-----------------------------------------------------------------------------
// The screen that's displayed when the sketch fails to solve. A report of
// what failed, and (if the problem is a singular Jacobian) a list of
// constraints that could be removed to fix it.
//-----------------------------------------------------------------------------
void TextWindow::ScreenAllowRedundant(int link, uint32_t v) {
    SS.UndoRemember();

    Group *g = SK.GetGroup(SS.TW.shown.group);
    g->allowRedundant = true;
    SS.MarkGroupDirty(SS.TW.shown.group);

    SS.TW.shown.screen = Screen::GROUP_INFO;
    SS.TW.Show();
}
void TextWindow::ShowGroupSolveInfo() {
    Group *g = SK.GetGroup(shown.group);
    if(g->IsSolvedOkay()) {
        // Go back to the default group info screen
        shown.screen = Screen::GROUP_INFO;
        Show();
        return;
    }

    Printf(true, "%Ft%s   %E%s", _("GROUP"), g->DescriptionString().c_str());
    switch(g->solved.how) {
        case SolveResult::DIDNT_CONVERGE:
            Printf(true, "%Fx%s%Fd %s", _("SOLVE FAILED!"), _("unsolvable constraints"));
            Printf(true, "%s", _("the following constraints are incompatible"));
            break;

        case SolveResult::REDUNDANT_DIDNT_CONVERGE:
            Printf(true, "%Fx%s%Fd %s", _("SOLVE FAILED!"), _("unsolvable constraints"));
            Printf(true, "%s", _("the following constraints are unsatisfied"));
            break;

        case SolveResult::REDUNDANT_OKAY:
            Printf(true, "%Fx%s%Fd %s", _("SOLVE FAILED!"), _("redundant constraints"));
            Printf(true, "%s", _("remove any one of these to fix it"));
            break;

        case SolveResult::TOO_MANY_UNKNOWNS:
            Printf(true, "%s", _("Too many unknowns in a single group!"));
            return;

        default: ssassert(false, "Unexpected solve result");
    }

    for(int i = 0; i < g->solved.remove.n; i++) {
        hConstraint hc = g->solved.remove.elem[i];
        Constraint *c = SK.constraint.FindByIdNoOops(hc);
        if(!c) continue;

        Printf(false, "%Bp   %Fl%Ll%D%f%h%s%E",
            (i & 1) ? 'd' : 'a',
            c->h.v, (&TextWindow::ScreenSelectConstraint),
            (&TextWindow::ScreenHoverConstraint),
            c->DescriptionString().c_str());
    }

    Printf(false, "%s", _("It may be possible to fix the problem"));
    Printf(false, "%s", _("by selecting Edit -> Undo."));

    if(g->solved.how == SolveResult::REDUNDANT_OKAY) {
        Printf(true,  "%s", _("It is possible to suppress this error"));
        Printf(false, "%s", _("by allowing redundant constraints in "),
                      &TextWindow::ScreenAllowRedundant);
        Printf(false, "%s", _("this group."));
    }
}

//-----------------------------------------------------------------------------
// When we're stepping a dimension. User specifies the finish value, and
// how many steps to take in between current and finish, re-solving each
// time.
//-----------------------------------------------------------------------------
void TextWindow::ScreenStepDimFinish(int link, uint32_t v) {
    SS.TW.edit.meaning = Edit::STEP_DIM_FINISH;
    std::string edit_value;
    if(SS.TW.shown.dimIsDistance) {
        edit_value = SS.MmToString(SS.TW.shown.dimFinish);
    } else {
        edit_value = ssprintf("%.3f", SS.TW.shown.dimFinish);
    }
    SS.TW.ShowEditControl(12, edit_value);
}
void TextWindow::ScreenStepDimSteps(int link, uint32_t v) {
    SS.TW.edit.meaning = Edit::STEP_DIM_STEPS;
    SS.TW.ShowEditControl(12, ssprintf("%d", SS.TW.shown.dimSteps));
}
void TextWindow::ScreenStepDimGo(int link, uint32_t v) {
    hConstraint hc = SS.TW.shown.constraint;
    Constraint *c = SK.constraint.FindByIdNoOops(hc);
    if(c) {
        SS.UndoRemember();
        double start = c->valA, finish = SS.TW.shown.dimFinish;
        int i, n = SS.TW.shown.dimSteps;
        for(i = 1; i <= n; i++) {
            c = SK.GetConstraint(hc);
            c->valA = start + ((finish - start)*i)/n;
            SS.MarkGroupDirty(c->group);
            SS.GenerateAll();
            if(!SS.ActiveGroupsOkay()) {
                // Failed to solve, so quit
                break;
            }
            PaintGraphics();
        }
    }
    InvalidateGraphics();
    SS.TW.GoToScreen(Screen::LIST_OF_GROUPS);
}
void TextWindow::ShowStepDimension() {
    Constraint *c = SK.constraint.FindByIdNoOops(shown.constraint);
    if(!c) {
        shown.screen = Screen::LIST_OF_GROUPS;
        Show();
        return;
    }

    Printf(true, "%Ft%s%E %s", _("STEP DIMENSION"), c->DescriptionString().c_str());

    if(shown.dimIsDistance) {
        Printf(true,  "%Ba   %Ft%s%E    %s", _("start"), SS.MmToString(c->valA).c_str());
        Printf(false, "%Bd   %Ft%s%E   %s %Fl%Ll%f[%s]%E", _("finish"),
            SS.MmToString(shown.dimFinish).c_str(), &ScreenStepDimFinish, _("change"));
    } else {
        Printf(true,  "%Ba   %Ft%s%E    %@", _("start"), c->valA);
        Printf(false, "%Bd   %Ft%s%E   %@ %Fl%Ll%f[%s]%E", _("finish"),
            shown.dimFinish, &ScreenStepDimFinish, _("change"));
    }
    Printf(false, "%Ba   %Ft%s%E    %d %Fl%Ll%f%D[%s]%E", _("steps"),
        shown.dimSteps, &ScreenStepDimSteps, _("change"));

    Printf(true, " %Fl%Ll%f%s%E", &ScreenStepDimGo, _("step dimension now"));

    Printf(true, "(or %Fl%Ll%f%s%E)", &ScreenHome, _("cancel operation"));
}

//-----------------------------------------------------------------------------
// When we're creating tangent arcs (as requests, not as some parametric
// thing). User gets to specify the radius, and whether the old untrimmed
// curves are kept or deleted.
//-----------------------------------------------------------------------------
void TextWindow::ScreenChangeTangentArc(int link, uint32_t v) {
    switch(link) {
        case 'r': {
            SS.TW.edit.meaning = Edit::TANGENT_ARC_RADIUS;
            SS.TW.ShowEditControl(3, SS.MmToString(SS.tangentArcRadius));
            break;
        }

        case 'a': SS.tangentArcManual = !SS.tangentArcManual; break;
        case 'd': SS.tangentArcDeleteOld = !SS.tangentArcDeleteOld; break;
    }
}
void TextWindow::ShowTangentArc() {
    Printf(true, "%Ft%s%E", _("TANGENT ARC PARAMETERS"));

    Printf(true,  "%Ft %s%E", _("radius of created arc"));
    if(SS.tangentArcManual) {
        Printf(false, "%Ba   %s %Fl%Lr%f[%s]%E",
            SS.MmToString(SS.tangentArcRadius).c_str(),
            &(TextWindow::ScreenChangeTangentArc), _("change"));
    } else {
        Printf(false, "%Ba   %s", _("automatic"));
    }

    Printf(false, "");
    Printf(false, "  %Fd%f%La%s  %s%E",
        &ScreenChangeTangentArc,
        !SS.tangentArcManual ? CHECK_TRUE : CHECK_FALSE,
        _("choose radius automatically"));
    Printf(false, "  %Fd%f%Ld%s  %s%E",
        &ScreenChangeTangentArc,
        SS.tangentArcDeleteOld ? CHECK_TRUE : CHECK_FALSE,
        _("delete original entities afterward"));

    Printf(false, "");
    Printf(false, "%s", _("To create a tangent arc at a point,"));
    Printf(false, "%s", _("select that point and then choose"));
    Printf(false, "%s", _("Sketch -> Tangent Arc at Point."));
    Printf(true, "(%s %Fl%Ll%f%s%E)", _("or"), &ScreenHome,
    	_("back to home screen"));
}

//-----------------------------------------------------------------------------
// The edit control is visible, and the user just pressed enter.
//-----------------------------------------------------------------------------
void TextWindow::EditControlDone(const char *s) {
    edit.showAgain = false;

    switch(edit.meaning) {
        case Edit::TIMES_REPEATED: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) {
                SS.UndoRemember();

                double ev = e->Eval();
                if((int)ev < 1) {
                    Error(_("Can't repeat fewer than 1 time."));
                    break;
                }
                if((int)ev > 999) {
                    Error(_("Can't repeat more than 999 times."));
                    break;
                }

                Group *g = SK.GetGroup(edit.group);
                g->valA = ev;

                if(g->type == Group::Type::ROTATE) {
                    int i, c = 0;
                    for(i = 0; i < SK.constraint.n; i++) {
                        if(SK.constraint.elem[i].group.v == g->h.v) c++;
                    }
                    // If the group does not contain any constraints, then
                    // set the numerical guess to space the copies uniformly
                    // over one rotation. Don't touch the guess if we're
                    // already constrained, because that would break
                    // convergence.
                    if(c == 0) {
                        double copies = (g->skipFirst) ? (ev + 1) : ev;
                        SK.GetParam(g->h.param(3))->val = PI/(2*copies);
                    }
                }

                SS.MarkGroupDirty(g->h);
            }
            break;
        }
        case Edit::GROUP_NAME: {
            if(!*s) {
                Error(_("Group name cannot be empty"));
            } else {
                SS.UndoRemember();

                Group *g = SK.GetGroup(edit.group);
                g->name = s;
            }
            break;
        }
        case Edit::GROUP_SCALE: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) {
                double ev = e->Eval();
                if(fabs(ev) < 1e-6) {
                    Error(_("Scale cannot be zero."));
                } else {
                    Group *g = SK.GetGroup(edit.group);
                    g->scale = ev;
                    SS.MarkGroupDirty(g->h);
                }
            }
            break;
        }
        case Edit::GROUP_COLOR: {
            Vector rgb;
            if(sscanf(s, "%lf, %lf, %lf", &rgb.x, &rgb.y, &rgb.z)==3) {
                rgb = rgb.ClampWithin(0, 1);

                Group *g = SK.group.FindByIdNoOops(SS.TW.shown.group);
                if(!g) break;
                g->color = RGBf(rgb.x, rgb.y, rgb.z);

                SS.MarkGroupDirty(g->h);
                SS.GW.ClearSuper();
            } else {
                Error(_("Bad format: specify color as r, g, b"));
            }
            break;
        }
        case Edit::GROUP_OPACITY: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(e) {
                double alpha = e->Eval();
                if(alpha < 0 || alpha > 1) {
                    Error(_("Opacity must be between zero and one."));
                } else {
                    Group *g = SK.GetGroup(edit.group);
                    g->color.alpha = (int)(255.1f * alpha);
                    SS.MarkGroupDirty(g->h);
                    SS.GW.ClearSuper();
                }
            }
            break;
        }
        case Edit::TTF_TEXT: {
            SS.UndoRemember();
            Request *r = SK.request.FindByIdNoOops(edit.request);
            if(r) {
                r->str = s;
                SS.MarkGroupDirty(r->group);
            }
            break;
        }
        case Edit::STEP_DIM_FINISH: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(!e) {
                break;
            }
            if(shown.dimIsDistance) {
                shown.dimFinish = SS.ExprToMm(e);
            } else {
                shown.dimFinish = e->Eval();
            }
            break;
        }
        case Edit::STEP_DIM_STEPS:
            shown.dimSteps = min(300, max(1, atoi(s)));
            break;

        case Edit::TANGENT_ARC_RADIUS: {
            Expr *e = Expr::From(s, /*popUpError=*/true);
            if(!e) break;
            if(e->Eval() < LENGTH_EPS) {
                Error(_("Radius cannot be zero or negative."));
                break;
            }
            SS.tangentArcRadius = SS.ExprToMm(e);
            break;
        }

        default: {
            int cnt = 0;
            if(EditControlDoneForStyles(s))         cnt++;
            if(EditControlDoneForConfiguration(s))  cnt++;
            if(EditControlDoneForPaste(s))          cnt++;
            if(EditControlDoneForView(s))           cnt++;
            ssassert(cnt == 1, "Expected exactly one parameter to be edited");
            break;
        }
    }
    InvalidateGraphics();
    SS.ScheduleShowTW();

    if(!edit.showAgain) {
        HideEditControl();
        edit.meaning = Edit::NOTHING;
    }
}

