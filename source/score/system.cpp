/*
  * Copyright (C) 2013 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "system.h"

#include <algorithm>
#include <boost/foreach.hpp>
#include <cstddef>
#include <limits>
#include "utils.h"

namespace Score {

using namespace Detail;

System::System()
{
    // Add the start and end bars.
    myBarlines.push_back(Barline());
    Barline endBar;
    endBar.setPosition(std::numeric_limits<int>::max());
    myBarlines.push_back(endBar);
}

bool System::operator==(const System &other) const
{
    return myStaves == other.myStaves &&
           myBarlines == other.myBarlines &&
           myTempoMarkers == other.myTempoMarkers &&
           myAlternateEndings == other.myAlternateEndings;
}

boost::iterator_range<System::StaffIterator> System::getStaves()
{
    return boost::make_iterator_range(myStaves);
}

boost::iterator_range<System::StaffConstIterator> System::getStaves() const
{
    return boost::make_iterator_range(myStaves);
}

void System::insertStaff(const Staff &staff)
{
    myStaves.push_back(staff);
}

void System::removeStaff(const Staff &staff)
{
    myStaves.erase(std::remove(myStaves.begin(), myStaves.end(), staff),
                   myStaves.end());
}

boost::iterator_range<System::BarlineIterator> System::getBarlines()
{
    return boost::make_iterator_range(myBarlines);
}

boost::iterator_range<System::BarlineConstIterator> System::getBarlines() const
{
    return boost::make_iterator_range(myBarlines);
}

void System::insertBarline(const Barline &barline)
{
    insertObject(myBarlines, barline);
}

void System::removeBarline(const Barline &barline)
{
    removeObject(myBarlines, barline);
}

const Barline *System::getPreviousBarline(int position) const
{
    BOOST_REVERSE_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() <= position)
            return &barline;
    }

    return NULL;
}

const Barline *System::getNextBarline(int position) const
{
    BOOST_FOREACH(const Barline &barline, myBarlines)
    {
        if (barline.getPosition() >= position)
            return &barline;
    }

    return NULL;
}

boost::iterator_range<System::TempoMarkerIterator> System::getTempoMarkers()
{
    return boost::make_iterator_range(myTempoMarkers);
}

boost::iterator_range<System::TempoMarkerConstIterator> System::getTempoMarkers() const
{
    return boost::make_iterator_range(myTempoMarkers);
}

void System::insertTempoMarker(const TempoMarker &marker)
{
    insertObject(myTempoMarkers, marker);
}

void System::removeTempoMarker(const TempoMarker &marker)
{
    removeObject(myTempoMarkers, marker);
}

boost::iterator_range<System::AlternateEndingIterator> System::getAlternateEndings()
{
    return boost::make_iterator_range(myAlternateEndings);
}

boost::iterator_range<System::AlternateEndingConstIterator> System::getAlternateEndings() const
{
    return boost::make_iterator_range(myAlternateEndings);
}

void System::insertAlternateEnding(const AlternateEnding &ending)
{
    insertObject(myAlternateEndings, ending);
}

void System::removeAlternateEnding(const AlternateEnding &ending)
{
    removeObject(myAlternateEndings, ending);
}

}
