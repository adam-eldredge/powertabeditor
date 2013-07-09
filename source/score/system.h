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

#ifndef SCORE_SYSTEM_H
#define SCORE_SYSTEM_H

#include <boost/serialization/access.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <vector>
#include "barline.h"

namespace Score {

class System
{
public:
    typedef std::vector<Barline>::iterator BarlineIterator;
    typedef std::vector<Barline>::const_iterator BarlineConstIterator;

    System();

    /// Returns the set of barlines in the system.
    boost::iterator_range<BarlineIterator> getBarlines();
    /// Returns the set of barlines in the system.
    boost::iterator_range<BarlineConstIterator> getBarlines() const;

    /// Adds a new barline to the system.
    void insertBarline(const Barline &barline);
    /// Removes the specified barline from the system.
    void removeBarline(const Barline &barline);

    /// Returns the barline at the given position index in the system.
    const Barline *getBarlineAtPosition(int position) const;
    Barline *getBarlineAtPosition(int position);

    /// Returns the last barline before (or including) the given position.
    const Barline *getPreviousBarline(int position) const;

    /// Returns the first barline after (or including) the given position.
    const Barline *getNextBarline(int position) const;

private:
    /// List of the barlines in the system. This will always contain at least
    /// two barlines - the start and end bars.
    std::vector<Barline> myBarlines;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & myBarlines;
    }
};

}

#endif
