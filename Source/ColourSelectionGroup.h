/*
  ==============================================================================

    ColourSelectionGroup.h
    Created: 20 Dec 2020 5:40:13pm
    Author:  Vincenzo

    Base classes for maintaining multiple objects that resolve to one or zero colours being selected.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ColourSelectionBroadcaster;

class ColourSelectionListener
{
public:
    virtual void colourChangedCallback(ColourSelectionBroadcaster* source, Colour newColour) = 0;
};

class ColourSelectionBroadcaster
{
public:

    ColourSelectionBroadcaster() {};
    virtual ~ColourSelectionBroadcaster() {};

    virtual Colour getSelectedColour() = 0;

    virtual void deselectColour() = 0;

    void    addColourSelectionListener(ColourSelectionListener* listenerIn) { selectorListeners.add(listenerIn); }
    void removeColourSelectionListener(ColourSelectionListener* listenerIn) { selectorListeners.add(listenerIn); }

protected:

    ListenerList<ColourSelectionListener> selectorListeners;
};

class ColourSelectionGroup :  public ColourSelectionBroadcaster,
                              private ColourSelectionListener
{
public:

    /// <summary>
    /// Adds a ColourPaletteComponent and returns the palette's group index
    /// </summary>
    /// <param name="paletteComponentToAdd"></param>
    /// <returns></returns>
    int addSelector(ColourSelectionBroadcaster* selectorToAdd)
    {
        bool newSelector = colourSelectors.addIfNotAlreadyThere(selectorToAdd);

        if (newSelector)
            selectorToAdd->addColourSelectionListener(this);

        return colourSelectors.indexOf(selectorToAdd);
    }

    void removeSelector(ColourSelectionBroadcaster* paletteComponentToRemove)
    {
        int removed = colourSelectors.removeAllInstancesOf(paletteComponentToRemove);

        if (removed > 0)
            paletteComponentToRemove->removeColourSelectionListener(this);
    }

    /// <summary>
    /// Removes the selected status from the selected palette swatch
    /// </summary>
    void deselectColour() override
    {
        if (selectedBroadcasterIndex >= 0)
            colourSelectors[selectedBroadcasterIndex]->deselectColour();

        selectedBroadcasterIndex = -1;
    }

    Colour getSelectedColour() override
    {
        if (selectedBroadcasterIndex >= 0)
            return colourSelectors[selectedBroadcasterIndex]->getSelectedColour();
        
        return Colour();
    }

    ColourSelectionBroadcaster* getSelectedBroadcaster() const
    {
        if (selectedBroadcasterIndex >= 0)
            return colourSelectors[selectedBroadcasterIndex];

        return nullptr;
    }

    int getSelectedBroadcasterIndex() const
    {
        return selectedBroadcasterIndex;
    }

    int getIndexOfSelector(ColourSelectionBroadcaster* selectorIn) const
    {
        return colourSelectors.indexOf(selectorIn);
    }

    //=========================================================================

    /// <summary>
    /// Use this callback to determine the selected colour among all colour selectors
    /// </summary>
    /// <param name="source"></param>
    /// <param name="newColour"></param>
    void colourChangedCallback(ColourSelectionBroadcaster* source, Colour newColour) override
    {
        if (getSelectedBroadcaster() && getSelectedBroadcaster() != source)
        {
            getSelectedBroadcaster()->deselectColour();
        }

        selectedBroadcasterIndex = colourSelectors.indexOf(source);

        selectorListeners.call(&ColourSelectionListener::colourChangedCallback, source, newColour);
    }

private:

    Array<ColourSelectionBroadcaster*> colourSelectors;

    int selectedBroadcasterIndex = -1;
};