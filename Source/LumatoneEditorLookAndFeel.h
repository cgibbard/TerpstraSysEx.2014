/*
  ==============================================================================

    LumatoneEditorLookAndFeel.h
    Created: 13 Nov 2020 7:25:04pm
    Author:  Vincenzo Sicurella

    Asset loading, helper functions, and a minimal implementation of LookAndFeel_V4
    for the Lumatone Editor application.
    
    Some components also need to be passed into its respective "Setup" function 
    to apply default colours or other properties.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==================================================================
//
// STATIC HELPERS
//
//==================================================================

// JUCE methods' radians are rotated -90deg with respect to trigonometry functions
static const float PATH_PI_2_CW = float_Pi / 2;
static const float PATH_PI_2_CCW = -float_Pi / 2;

/// <summary>
/// Returns a path representing a rectangle shape, considering rounded corners and connected edges
/// </summary>
/// <param name="bounds"></param>
/// <param name="roundedCornerSize"></param>
/// <param name="connectedFlags"></param>
/// <returns></returns>
static Path getConnectedRoundedRectPath(Rectangle<float> bounds, float roundedCornerSize, int connectedFlags)
{
    Path rect;

    if (connectedFlags == 0 || connectedFlags == 3 || connectedFlags == 12 || connectedFlags == 15)
    {
        rect.addRoundedRectangle(bounds, roundedCornerSize);
        return rect;
    }

    roundedCornerSize *= 2;

    int xTo, yTo;

    if (connectedFlags & Button::ConnectedEdgeFlags::ConnectedOnTop || connectedFlags & Button::ConnectedOnLeft)
    {
        rect.startNewSubPath(0, 0);
    }
    else
    {
        rect.addArc(0, 0, roundedCornerSize, roundedCornerSize, PATH_PI_2_CCW, 0, true);
    }

    if (connectedFlags & Button::ConnectedEdgeFlags::ConnectedOnTop || connectedFlags & Button::ConnectedEdgeFlags::ConnectedOnRight)
    {
        rect.lineTo(bounds.getWidth(), 0);
    }
    else
    {
        xTo = bounds.getWidth() - roundedCornerSize;
        rect.lineTo(xTo, 0);
        rect.addArc(xTo, 0, roundedCornerSize, roundedCornerSize, 0, PATH_PI_2_CW);
    }

    if (connectedFlags & Button::ConnectedEdgeFlags::ConnectedOnRight || connectedFlags & Button::ConnectedEdgeFlags::ConnectedOnBottom)
    {
        rect.lineTo(bounds.getWidth(), bounds.getHeight());
    }
    else
    {
        yTo = bounds.getHeight() - roundedCornerSize;
        rect.lineTo(bounds.getWidth(), yTo);
        rect.addArc(bounds.getWidth() - roundedCornerSize, yTo, roundedCornerSize, roundedCornerSize, PATH_PI_2_CW, float_Pi);
    }

    if (connectedFlags & Button::ConnectedEdgeFlags::ConnectedOnBottom || connectedFlags & Button::ConnectedEdgeFlags::ConnectedOnLeft)
    {
        rect.lineTo(0, bounds.getHeight());
    }
    else
    {
        rect.lineTo(roundedCornerSize, bounds.getHeight());
        rect.addArc(0, bounds.getHeight() - roundedCornerSize, roundedCornerSize, roundedCornerSize, -float_Pi, PATH_PI_2_CCW);
    }

    rect.closeSubPath();
    return rect;
}

/// <summary>
/// Adds an arc to a Path using a juce::Rectangle object for bounds.
/// </summary>
/// <param name="pathIn"></param>
/// <param name="ellipseBounds"></param>
/// <param name="fromRadians"></param>
/// <param name="toRadians"></param>
/// <param name="startAsNewSubPath"></param>
static void addArcToPath(Path& pathIn, Rectangle<float>& ellipseBounds, float fromRadians, float toRadians, bool startAsNewSubPath)
{
    pathIn.addArc(
        ellipseBounds.getX(),
        ellipseBounds.getY(),
        ellipseBounds.getWidth(),
        ellipseBounds.getHeight(),
        fromRadians,
        toRadians,
        startAsNewSubPath
    );
}

/// <summary>
/// Returns a rect with the given proportions relative to the parentRect
/// </summary>
/// <param name="parentRect"></param>
/// <param name="rectProportions"></param>
/// <returns></returns>
static Rectangle<float> getProportionalRectFrom(const Rectangle<float>& parentRect, const Rectangle<float>& rectProportions)
{
    return Rectangle<float>(
        parentRect.getWidth() * rectProportions.getX() + parentRect.getX(),
        parentRect.getHeight() * rectProportions.getY() + parentRect.getY(),
        parentRect.getWidth() * rectProportions.getWidth(),
        parentRect.getHeight() * rectProportions.getHeight()
        );
}

/// <summary>
/// Intended for use with components in FlexBoxes, this scales a component and sets the justifications within the FlexBox
/// </summary>
/// <param name="component"></param>
/// <param name="scaleFactorX"></param>
/// <param name="scaleFactorY"></param>
/// <param name="justification"></param>
static void scaleAndJustifyComponent(Component& component, float scaleFactorX, float scaleFactorY, Justification justification)
{
    int newWidth = round(component.getWidth() * scaleFactorX);
    int newHeight = round(component.getHeight() * scaleFactorY);

    int xOff, yOff;

    if (justification.getFlags() & Justification::left)
    {
        xOff = 0;
    }
    else
    {
        if (justification.getFlags() & Justification::right | Justification::centred)
        {
            xOff = component.getWidth() - newWidth;
        }

        if (justification.getFlags() & Justification::horizontallyCentred)
        {
            xOff = round(xOff / 2.0f);
        }
    }

    if (justification.getFlags() & Justification::top)
    {
        yOff = 0;
    }
    else
    {
        if (justification.getFlags() & Justification::bottom | Justification::centred)
        {
            yOff = component.getHeight() - newHeight;
        }

        if (justification.getFlags() & Justification::verticallyCentred)
        {
            yOff = round(yOff / 2.0f);
        }
    }

    component.setBounds(component.getX() + xOff, component.getY() + yOff, newWidth, newHeight);
}

/// <summary>
/// Intended for use with components in FlexBoxes, this scales a component and sets the justifications within the FlexBox
/// </summary>
/// <param name="component"></param>
/// <param name="scaleFactor"></param>
/// <param name="justification"></param>
static void scaleAndJustifyComponent(Component& component, float scaleFactor, Justification justification = Justification::centred)
{
    scaleAndJustifyComponent(component, scaleFactor, scaleFactor, justification);
}

/// <summary>
/// Scales a component based on the size of a given image
/// </summary>
/// <param name="component"></param>
/// <param name="image"></param>
/// <param name="scaleFactorX"></param>
/// <param name="scaleFactorY"></param>
static void scaleBySourceImage(Component* component, const Image& image, float scaleFactorX, float scaleFactorY)
{
    Rectangle<int> bounds = image.getBounds();
    component->setSize(bounds.getWidth() * scaleFactorX, bounds.getHeight() * scaleFactorY);
}

/// <summary>
/// Scales an ImageComponent based on the size of it's image
/// </summary>
/// <param name="component"></param>
/// <param name="scaleFactorX"></param>
/// <param name="scaleFactorY"></param>
static void scaleBySourceImage(ImageComponent* component, float scaleFactorX, float scaleFactorY)
{
    scaleBySourceImage((Component*)component, component->getImage(), scaleFactorX, scaleFactorY);
}

/// <summary>
/// Scales an ImageComponent based on the size of it's image
/// </summary>
/// <param name="component"></param>
/// <param name="scaleFactor"></param>
static void scaleBySourceImage(ImageComponent* component, float scaleFactor)
{
    scaleBySourceImage(component, scaleFactor, scaleFactor);
}

/// <summary>
/// Scales an ImageButton based on the size of it's image
/// </summary>
/// <param name="component"></param>
/// <param name="scaleFactorX"></param>
/// <param name="scaleFactorY"></param>
static void scaleBySourceImage(ImageButton* component, float scaleFactorX, float scaleFactorY)
{
    scaleBySourceImage(component, component->getNormalImage(), scaleFactorX, scaleFactorY);
}

/// <summary>
/// Scales an ImageButton based on the size of it's image
/// </summary>
/// <param name="component"></param>
/// <param name="scaleFactorX"></param>
/// <param name="scaleFactorY"></param>
static void scaleBySourceImage(ImageButton* component, float scaleFactor)
{
    scaleBySourceImage(component, component->getNormalImage(), scaleFactor, scaleFactor);
}

// Creates the Lumatone logomark in 1x1 floating bit path - the LookAndFeel class will do this during construction
static Path createLogomark()
{
    Path logo;
    Point<float> center(0.5f, 0.5f);

    float phi2 = 1 + sqrtf(5);
    float innerRad = 1.0f / phi2;
    float outerRad = phi2 / 8.0f;
    float ang = float_Pi / 12.0f;
    float angOff = ang / 2.0f;

    logo.addPolygon(center, 6, innerRad, ang - angOff);
    logo.addPolygon(center, 6, sqrt(3) / 2 * innerRad, -ang - angOff);
    logo.addPolygon(center, 6, outerRad, ang - angOff);
    logo.addPolygon(center, 6, outerRad, -ang - angOff);

    return logo;
}


// Hash codes for use with ImageCache::getFromHashCode()
enum LumatoneEditorAssets
{
    LumatoneTitle       = 0x0001000,
    PleaseConnect       = 0x0001001,
    SelectMidiInput     = 0x0001002,
    SelectMidiOutput    = 0x0001003,
    Disconnected        = 0x0001004,
    Connected           = 0x0001005,
    EditMode            = 0x0001006,
    LiveEditor          = 0x0001007,
    OfflineEditor       = 0x0001008,
    LoadFile            = 0x0002000,
    SaveFile            = 0x0002001,
    ImportPreset        = 0x0002002,
    LumatoneGraphic     = 0x0002100,
    KeybedShadows       = 0x0002101,
    KeyShape            = 0x0002200,
    KeyShadow           = 0x0002201,
    AssignKeys          = 0x0003000,
    GeneralSettings     = 0x0003001,
    ExpressionPedal     = 0x0003002,
    Curves              = 0x0003003,
    TickBox             = 0x0003100,
    PresetButtonColours = 0x0004000,
    SavePalette         = 0x0005000,
    CancelPalette       = 0x0005001,
    TrashCanIcon        = 0x0005002
};


class LumatoneEditorLookAndFeel : public LookAndFeel_V4
{
public:

    LumatoneEditorLookAndFeel()
    {
        franklinGothicMedium.setTypefaceStyle("Medium");

        gothamNarrowBook = gothamNarrowMedium.withTypefaceStyle("Book");
        gothamNarrowMedium.setTypefaceStyle("Narrow Medium");
        gothamNarrowMediumBold = gothamNarrowMedium.withTypefaceStyle("Narrow Medium Bold");
        gothamNarrowMediumItalic = gothamNarrowMedium.withTypefaceStyle("Narrow Medium Italic");

        logomarkPath = createLogomark();
    }

public:
    //==================================================================
    // LookAndFeel_V4 Implementation

    //==================================================================
    //
    // BUTTON METHODS
    //
    //==================================================================

    void drawButtonBackground(Graphics& g, Button& btn, const Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        Colour colour = (shouldDrawButtonAsDown) ? btn.findColour(TextButton::ColourIds::buttonOnColourId) : backgroundColour;

        if (shouldDrawButtonAsHighlighted)
            colour = colour.brighter(0.075f);
        
        g.setColour(colour);
        g.fillPath(getButtonShape(btn));
    }

    void drawImageButton(Graphics& g, Image* img, int imageX, int imageY, int imageW, int imageH, const Colour& overlayColour, float imageOpacity, ImageButton& btn) override
    {
        Path buttonShape = getButtonShape(btn);

        Colour colour = Colour(0xff383b3d);
        Colour textColour = Colours::white;

        if (!btn.getToggleState())
        {
            colour = Colour(0xff1c1c1c);
            textColour = Colour(0xffb5b5b5);
        }

        g.setColour(colour);
        g.fillPath(buttonShape);
        
        g.setColour(textColour);
        g.drawImageWithin(*img, 0, btn.getHeight() / 3, btn.getWidth(), btn.getHeight() / 3, RectanglePlacement::centred, btn.getClickingTogglesState());

        if (!btn.getClickingTogglesState() || !btn.getToggleState())
        {
            if (btn.getToggleState() && btn.isMouseButtonDown())
            {
                g.setColour(Colours::black.withAlpha(0.1f));
                g.fillPath(buttonShape);
            }

            else if (btn.isMouseOver())
            {
                g.setColour(Colours::white.withAlpha(0.07f));
                g.fillPath(buttonShape);
            }
        }
    }

    Font getTextButtonFont(TextButton&, int buttonHeight) override
    {
        return gothamNarrowMedium.withHeight(buttonHeight / 2);
    }

    void drawTickBox(Graphics& g, Component& c, float x, float y, float w, float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        Path shape;
        shape.addRoundedRectangle(x, y, w, h, w / 10.0f);
       
        g.setColour(Colours::white);
        g.fillPath(shape);

        // TODO: Proper "tick" drawing
        if (ticked)
        {
            g.setColour(Colours::black);
            g.setFont(gothamNarrowMedium.withHeight(h).withHorizontalScale(4.0f / 3.0f));
            g.drawFittedText("X", x, y, w, h, Justification::centred, 1, 1.0f);
        }
    }   

    void drawToggleButton(Graphics& g, ToggleButton& btn, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        drawTickBox(g, btn, 0, 0, btn.getHeight(), btn.getHeight(), btn.getToggleState(), btn.isEnabled(),
            shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        g.setFont(gothamNarrowMedium.withHeight(btn.getHeight()));

        Colour textColour = btn.findColour(ToggleButton::ColourIds::textColourId);
        
        if (shouldDrawButtonAsHighlighted)
            textColour = textColour.brighter(0.1f);
        
        if (shouldDrawButtonAsDown)
            textColour = textColour.darker();

        g.setColour(textColour);
        g.drawFittedText(btn.getButtonText(), btn.getLocalBounds().withLeft(btn.getHeight() * 1.5f), Justification::centredLeft, 1);
    }

    //==================================================================
    //
    // SLIDER METHODS
    //
    //==================================================================

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider& sld) override
    {
        float thickness = 1 / 6.0f;
        float dialRadiusFactor = thickness / 3.0f;

        float endAngleNorm = rotaryAngleEnd - float_Pi / 2;

        int w = sld.getWidth();
        int h = round(sld.getHeight() * (1.5 - 0.5 * sinf(endAngleNorm)));
        
        float size = jmin(w, h);
        
        Rectangle<float> outerBounds = Rectangle<float>((w - size) / 2, (h - size + 5) / 2, size, size).reduced(size * dialRadiusFactor / 2 + 5);
        Rectangle<float> innerBounds = outerBounds.reduced(size * thickness);
        Point<float> center = outerBounds.getCentre();

        float radiusOuter = size / 2.0f;
        float radiusInner = radiusOuter * (1 - thickness);

        Path ring;
        addArcToPath(ring, outerBounds, rotaryAngleStart, rotaryAngleEnd, true);
        ring.lineTo(center.x + cosf(endAngleNorm) * radiusInner, center.y + sinf(endAngleNorm) * radiusInner);
        addArcToPath(ring, innerBounds, rotaryAngleEnd, rotaryAngleStart, false);
        ring.closeSubPath();

        ColourGradient grad = ColourGradient(rotaryGradientStart, outerBounds.getTopLeft(), rotaryGradientEnd, outerBounds.getTopRight(), false);
        g.setGradientFill(grad);
        g.fillPath(ring);

        g.setColour(Colours::white);

        float dialAng = (rotaryAngleEnd - rotaryAngleStart) * sliderPosProportional + rotaryAngleStart - float_Pi / 2;
        g.drawLine(Line<float>(center, { center.x + cosf(dialAng) * radiusOuter, center.y + sinf(dialAng) * radiusOuter }), 5.0f);

        // TODO: Make text box positioning more consistent when manipulating the bounds' aspect ratio
        sld.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, size / 5, sld.getHeight() / 6);
    }

    Label* createSliderTextBox(Slider& sld) override
    {
        Label* label = new Label(sld.getName() + "_ValueLabel");
        label->setColour(Label::ColourIds::textColourId, Colour(0xffcbcbcb));
        label->setText(String(sld.getValue()), dontSendNotification);
        //label->setBounds(sld.getBoundsInParent().withTrimmedTop(sld.getHeight() * 0.75f));
        label->setFont(gothamNarrowMedium.withHeight(sld.getHeight() / 8.0f));
        label->setJustificationType(Justification::centred);

        return label;
    }

    //==================================================================
    //
    // COMBOBOX METHODS
    //
    //==================================================================

    void drawComboBox(Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box) override
    {
        float margin = height / comboBoxRoundedCornerFactor;

        g.setColour(box.findColour(ComboBox::ColourIds::backgroundColourId));

        Path boxShape = getConnectedRoundedRectPath(box.getLocalBounds().toFloat(), margin, box.isPopupActive() ? Button::ConnectedEdgeFlags::ConnectedOnBottom : 0);
        g.fillPath(boxShape);

        g.setFont(getComboBoxFont(box));
        g.setColour(box.findColour(ComboBox::ColourIds::textColourId));

        if (&box == inputDeviceBox && box.getSelectedId() == 0)
        {
            g.drawImageWithin(
                ImageCache::getFromHashCode(LumatoneEditorAssets::SelectMidiInput),
                margin, round(height / 3.0f), width - margin * 2, round(height / 2.5f),
                RectanglePlacement::onlyReduceInSize + RectanglePlacement::xLeft
            );
        }
        else if (&box == outputDeviceBox && box.getSelectedId() == 0)
        {
            g.drawImageWithin(
                ImageCache::getFromHashCode(LumatoneEditorAssets::SelectMidiOutput),
                margin, round(height / 3.0f), width - margin * 2, round(height / 2.5f),
                RectanglePlacement::onlyReduceInSize + RectanglePlacement::xLeft
            );
        }
        else
        {
            g.drawFittedText(box.getText(), margin, 0, width - margin, height, Justification::centredLeft, 1, 1.0f);
        }

        if (buttonW > 0)
        {
            g.setFont(getComboBoxFont(box).withHorizontalScale(1.875f));
            g.drawFittedText("v", buttonX, buttonY, buttonW - margin, buttonH * 0.9f, Justification::centredRight, 1);
        }
    }

    Font getComboBoxFont(ComboBox& box) override
    {
        return gothamNarrowMedium.withHeight(box.getHeight() * comboBoxFontHeightFactor);
    }

    Label* createComboBoxTextBox(ComboBox& box) override
    {
        Label* l = new Label(box.getName(), box.getText());
        l->setFont(getComboBoxFont(box));
        return l;
    }

    void positionComboBoxText(ComboBox& box, Label& labelToPosition) override
    {
        labelToPosition.setBounds(box.getLocalBounds()
            .withTrimmedLeft(box.getHeight() / comboBoxRoundedCornerFactor / 2)
            .withWidth(getComboBoxFont(box).getStringWidth(box.getText()))
        );
    }

    // Had an easier time not using the Options-based methods

    //PopupMenu::Options getOptionsForComboBoxPopupMenu(ComboBox& box, Label& label) override
    //{
    //    float margin = box.getHeight() / comboBoxRoundedCornerFactor;
    //    Rectangle<int> boxPos = box.getScreenBounds();

    //    return PopupMenu::Options()
    //        .withParentComponent(&box)
    //        .withTargetComponent(box)
    //        .withMinimumWidth(box.getWidth())
    //        .withMaximumNumColumns(1)
    //        .withStandardItemHeight(label.getFont().getHeight())
    //        .withPreferredPopupDirection(PopupMenu::Options::PopupDirection::downwards);
    //}

    //==================================================================
    //
    // POPUPMENU METHODS
    //
    //==================================================================

    void drawPopupMenuBackground(Graphics& g, int width, int height) override
    {
        float margin = height / comboBoxRoundedCornerFactor;
        g.setColour(Colour(0xff212427));

        Path menuShape = getConnectedRoundedRectPath(Rectangle<float>(0, 0, width, height), margin / 2, Button::ConnectedEdgeFlags::ConnectedOnTop);
        g.fillPath(menuShape);
    }

    //void drawPopupMenuBackgroundWithOptions(Graphics& g, int width, int height, const PopupMenu::Options& options) override
    //{
    //    float margin = height / comboBoxRoundedCornerFactor;
    //    g.setColour(Colour(0xff212427));

    //    auto parent = getParentComponentForMenuOptions(options);

    //    Path menuShape = getButtonShape(Rectangle<int>(0, 0, parent->getWidth(), height), margin, Button::ConnectedEdgeFlags::ConnectedOnTop);
    //    g.fillPath(menuShape);
    //}

    void drawPopupMenuItem(
        Graphics& g, const Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, 
        const String& text, const String& shortcutKeyText, const Drawable* icon, const Colour* textColour) override
    {
        if (isHighlighted)
        {
            g.setColour(Colours::white.withAlpha(0.15f));
            g.fillRect(area);
        }

        g.setColour(Colour(0xffcbcbcb));
        g.setFont(getPopupMenuFont().withHeight(area.getHeight() * comboBoxFontHeightFactor));
        g.drawFittedText(text, area.withTrimmedLeft(area.getHeight() / comboBoxRoundedCornerFactor ), Justification::centredLeft, 1);
    }

    //void drawPopupMenuItemWithOptions(
    //    Graphics& g, const Rectangle<int>& area, bool isHighlighted, const PopupMenu::Item& item, const PopupMenu::Options& options) override
    //{
    //    if (isHighlighted)
    //    {
    //        g.setColour(Colours::white.withAlpha(0.15f));
    //        g.fillRect(area);
    //    }

    //    g.setColour(Colour(0xffcbcbcb));
    //    g.setFont(gothamNarrowMedium.withHeight(area.getHeight()));
    //    g.drawFittedText(item.text, area.withTrimmedLeft(area.getHeight() * 1.5 / comboBoxRoundedCornerFactor), Justification::centredLeft, 1);
    //}

    //void getIdealPopupMenuItemSizeWithOptions(const String& text, bool isSeparator, int standardMenuItemHeight, int& idealWidth, int& idealHeight, 
    //    const PopupMenu::Options& options) override
    //{
    //    auto parent = getParentComponentForMenuOptions(options);

    //    idealWidth = parent->getWidth();
    //    idealHeight = parent->getHeight();
    //}

    Font getPopupMenuFont() override
    {
        return gothamNarrowMedium;
    }

    int getMenuWindowFlags() override
    {
        return ComponentPeer::StyleFlags::windowIsSemiTransparent;
    }

    void preparePopupMenuWindow(Component& window) override
    {
        window.setOpaque(false);
    }

    //==================================================================
    //
    // TABBEDBUTTONBAR METHODS
    //
    //==================================================================

    int getTabButtonSpaceAroundImage() override { return 0; }

    int getTabButtonOverlap(int tabDepth) override { return 0; }

    int getTabButtonBestWidth(TabBarButton& tbb, int tabDepth) override 
    {
        return gothamNarrowMedium.withHeight(tabDepth).getStringWidth(tbb.getButtonText()) * 1.5f;
    }

    void drawTabButton(TabBarButton& tbb, Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        Colour c;

        if (tbb.isFrontTab())
            c = Colours::white;

        else if (isMouseOver)
            c = c.withAlpha(0.15f);

        g.setColour(c);
        g.drawLine(0, tbb.getHeight(), tbb.getWidth(), tbb.getHeight(), 3.0f);

        drawTabButtonText(tbb, g, isMouseOver, isMouseDown);
    }

    Font getTabButtonFont(TabBarButton& tbb, float height) override
    {
        return gothamNarrowMedium.withHeight(height);
    }

    void drawTabButtonText(TabBarButton& tbb, Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        Colour c = Colour(0xffb1b1b1);

        if (tbb.isFrontTab())
            c = Colours::white;

        else if (isMouseOver)
            c = c.brighter(0.15f);

        g.setColour(c);
        g.setFont(getTabButtonFont(tbb, 35));
        g.drawFittedText(tbb.getButtonText(), tbb.getTextArea(), Justification::centred, 1);

    }

    void drawTabAreaBehindFrontButton(TabbedButtonBar& tbb, Graphics& g, int w, int h) override {}


public:
    //==============================================================================================
    // Getters

    Font getFranklinGothic()
    {
        return franklinGothicMedium;
    }

    Font getGothamNarrowMedium()
    {
        return gothamNarrowMedium;
    }

    Font getGothamNarrowMediumBold()
    {
        return gothamNarrowMediumBold;
    }

    Font getGothamNarrowMediumItalic()
    {
        return gothamNarrowMediumItalic;
    }

    Font getGothamNarrowBook()
    {
        return gothamNarrowBook;
    }

    Path getLogomarkPath()
    {
        return logomarkPath;
    }

public:
    //==============================================================================================
    // Component Setup functions

    // Set colours of TextButton based on Live and Offline Editor buttons
    void setupTextButton(Button& btn)
    {
        btn.setColour(TextButton::ColourIds::buttonColourId, Colour(0xff1c1c1c));
        btn.setColour(TextButton::ColourIds::buttonOnColourId, Colour(0xff383b3d));
        btn.setColour(TextButton::ColourIds::textColourOffId, Colour(0xffb5b5b5));
        btn.setColour(TextButton::ColourIds::textColourOnId, Colour(0xffffffff));
    }

    // Set colours of Image button based on Live and Offline Editor Buttons
    // Use the same image for Normal, Highlighted, and Down, but apply alpha layer on Highlighted
    void setupImageButton(ImageButton& btn, Image& btnImage)
    {
        btn.setColour(TextButton::ColourIds::buttonColourId, Colour(0xff1c1c1c));
        btn.setColour(TextButton::ColourIds::buttonOnColourId, Colour(0xff383b3d));

        btn.setImages(false, true, true,
            btnImage, 1.0f, Colour(),
            btnImage, 1.0f, Colours::white.withAlpha(0.1f),
            btnImage, 1.0f, Colour()
        );
    }

    // Set text colour to be light gray
    void setupToggleButton(ToggleButton& btn)
    {
        btn.setColour(ToggleButton::ColourIds::textColourId, Colour(0xffcbcbcb));
    }

    // Set Slider style to Rotary
    void setupRotarySlider(Slider& sld)
    {
        sld.setSliderStyle(Slider::SliderStyle::Rotary);
    }

    // Set ComboBox colours and make internal reference for displaying "Select Midi In/Output" image asset 
    void setupMidiDeviceBox(ComboBox& box, bool isOutput)
    {
        box.setColour(ComboBox::ColourIds::backgroundColourId, Colour(0xff212427));
        box.setColour(ComboBox::ColourIds::textColourId, Colour(0xffd9e3e0));
        box.setColour(PopupMenu::ColourIds::backgroundColourId, Colour());
        box.setColour(PopupMenu::ColourIds::textColourId, Colours::red);// Colour(0xffd9e3e0));
        box.setTextWhenNothingSelected("");

        if (isOutput)
        {
            outputDeviceBox = &box;
        }
        else
        {
            inputDeviceBox = &box;
        }
    }

    // Set generic ComboBox colours
    void setupComboBox(ComboBox& box)
    {
        box.setColour(ComboBox::ColourIds::backgroundColourId, Colour(0xff212427));
        box.setColour(ComboBox::ColourIds::textColourId, Colour(0xffcbcbcb));
        box.setColour(PopupMenu::ColourIds::backgroundColourId, Colour());
        box.setColour(PopupMenu::ColourIds::textColourId, Colour(0xffd9e3e0));
    }


public:
    //==============================================================================================
    // Non-static drawing helpers

    // Button shape path helper with default rounded corner size
    Path getButtonShape(const Button& btn)
    {
        return getConnectedRoundedRectPath(btn.getLocalBounds().toFloat(), btn.getHeight() / buttonRoundedCornerFactor, btn.getConnectedEdgeFlags());
    }

    // Draws the Lumatone logomark within the given bounds
    void drawLogomarkWithin(Graphics& g, Rectangle<float> boundsToDrawIn)
    {
        g.strokePath(logomarkPath, PathStrokeType(0.8f), RectanglePlacement(RectanglePlacement::centred).getTransformToFit(
            logomarkPath.getBounds(),
            boundsToDrawIn
        ));
    }

public:
    //==============================================================================================
    
    /// <summary>
    /// MUST be called a single time when the application starts
    /// </summary>
    static void cacheImages()
    {
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::LumatoneTitle2x_png,          BinaryData::LumatoneTitle2x_pngSize),       LumatoneEditorAssets::LumatoneTitle);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::PleaseConnect2x_png,          BinaryData::PleaseConnect2x_pngSize),       LumatoneEditorAssets::PleaseConnect);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::MidiInput2x_png,              BinaryData::MidiInput2x_pngSize),           LumatoneEditorAssets::SelectMidiInput);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::MidiOutput2x_png,             BinaryData::MidiOutput2x_pngSize),          LumatoneEditorAssets::SelectMidiOutput);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::Disconnected2x_png,           BinaryData::Disconnected2x_pngSize),        LumatoneEditorAssets::Disconnected);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::EditMode2x_png,               BinaryData::EditMode2x_pngSize),            LumatoneEditorAssets::EditMode);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::LiveEditor2x_png,             BinaryData::LiveEditor2x_pngSize),          LumatoneEditorAssets::LiveEditor);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::OfflineEditor2x_png,          BinaryData::OfflineEditor2x_pngSize),       LumatoneEditorAssets::OfflineEditor);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::Connected2x_png,              BinaryData::Connected2x_pngSize),           LumatoneEditorAssets::Connected);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::ImportPreset2x_png,           BinaryData::ImportPreset2x_pngSize),        LumatoneEditorAssets::ImportPreset);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::SaveFile2x_png,               BinaryData::SaveFile2x_pngSize),            LumatoneEditorAssets::SaveFile);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::LoadFile2x_png,               BinaryData::LoadFile2x_pngSize),            LumatoneEditorAssets::LoadFile);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::AssignKeys2x_png,             BinaryData::AssignKeys2x_pngSize),          LumatoneEditorAssets::AssignKeys);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::GeneralSettings2x_png,        BinaryData::GeneralSettings2x_pngSize),     LumatoneEditorAssets::GeneralSettings);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::ExpressionPedal2x_png,        BinaryData::ExpressionPedal2x_pngSize),     LumatoneEditorAssets::ExpressionPedal);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::KeyboardBase_png,             BinaryData::KeyboardBase_pngSize),          LumatoneEditorAssets::LumatoneGraphic);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::KeybedShadows_png,            BinaryData::KeybedShadows_pngSize),         LumatoneEditorAssets::KeybedShadows);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::KeyShape2x_png,               BinaryData::KeyShape2x_pngSize),            LumatoneEditorAssets::KeyShape);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::KeyShadow2x_png,              BinaryData::KeyShadow2x_pngSize),           LumatoneEditorAssets::KeyShadow);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::Curves2x_png,                 BinaryData::Curves2x_pngSize),              LumatoneEditorAssets::Curves);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::PresetButtonColours2x_png,    BinaryData::PresetButtonColours2x_pngSize), LumatoneEditorAssets::PresetButtonColours);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::SavePalette2x_png,            BinaryData::SavePalette2x_pngSize),         LumatoneEditorAssets::SavePalette);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::CancelPalette2x_png,          BinaryData::CancelPalette2x_pngSize),       LumatoneEditorAssets::CancelPalette);
        ImageCache::addImageToCache(ImageCache::getFromMemory(BinaryData::TrashCanIcon2x_png,           BinaryData::TrashCanIcon2x_pngSize),        LumatoneEditorAssets::TrashCanIcon);
    }


private:

    // Default graphics constants

    const float buttonRoundedCornerFactor = 5.0f;

    const float comboBoxRoundedCornerFactor = 2.5f;
    const float comboBoxFontHeightFactor = 0.6f;

    const float rotaryAngleStart = float_Pi * -2 / 3.0f;
    const float rotaryAngleEnd = -rotaryAngleStart;

    const Colour rotaryGradientStart = Colour(84, 151, 182);
    const Colour rotaryGradientEnd = Colour(119, 168, 179);

    // Default Fonts

    Font univiaBoldPro = Font("Univia Pro", 12.0f, Font::bold);
    Font franklinGothicMedium = Font("Franklin Gothic", 12.0f, Font::plain);
    Font gothamNarrowMedium = Font("Gotham", 12.0f, Font::plain);
    Font gothamNarrowMediumBold;
    Font gothamNarrowMediumItalic;
    Font gothamNarrowBook;

    Path logomarkPath;

    // References for ComboBoxes with images

    ComboBox*  inputDeviceBox = nullptr;
    ComboBox* outputDeviceBox = nullptr;
};