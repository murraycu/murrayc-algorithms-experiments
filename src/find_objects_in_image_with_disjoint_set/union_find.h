/** Copyright (C) 2015 Murray Cumming
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef MURRAYC_UNION_FIND_H
#define MURRAYC_UNION_FIND_H

template
<typename T_Element = int>
class UnionFind
{
public:
  UnionFind(T_Element size)
  : id_(size)
  {
    for(int i = 0; i < size; ++i)
    {
      id_[i] = -1;
    }
  }

  void make_set(T_Element i)
  {
    id_[i] = i;
  }

  void union_set(T_Element p, T_Element q)
  {
    auto i = find_set(p);
    if(i == -1)
      i = p;

    auto j = find_set(q);
    if(j == -1)
      j = q;

    id_[i] = j;  
  }

  /**
   * This get the root.
   */
  T_Element find_set(T_Element i)
  {
    while(i != id_[i])
    {
      //TODO: Check for null:
      //id_[i] = id_[id_[i]];

      const auto id = id_[i];
      if(id == -1)
      {
        return id;
      }

      i = id;
    }

    return i;
  }

  std::vector<T_Element> id_;
};

#endif /* MURRAYC_UNION_FIND_H */
