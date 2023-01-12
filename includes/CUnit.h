/*
ACSE-4 Group 4.2 - Galena
First Created: 2021-03-23

Imperial College London
Department of Earth Science and Engineering

Group members:
    Iñigo Basterretxea Jacob
    Gordon Cheung
    Nina Kahr
    Miguel Pereira
    Ranran Tao
    Suyan Shi
    Jihao Xin
    Jie Zhu
*/

#ifndef _CUNIT_
#define _CUNIT_

// system includes
#include <exception>
#include <vector>

/*
The purpose of this class is to define separation unit objects which will be used
to make the circuit traversal more convenient.

@param conc: int, the unit index the concentrate flow will flow to
@param tails: int, the unit index the tailings flow will flow to
@param id: int, the index of the unit
@param parents, int (optional), index of parent, default to -1
*/

class SeparationUnit
{
    // ctors and dtors
    // since we don't need any custom copy ctors and dtors,
    // we will follow the rule of three and use the compiler
    // generated defaults for all copy and move ctors/assignment operators.
    // We also don't want to have a default ctor, only param ctor that throws
    // if the separation unit is defined incorrectly.
public:
    SeparationUnit(int conc, int tails, int id, int parent = -1);

    ~SeparationUnit() = default;

    class BadSeparationUnit : public std::exception
    {
        virtual const char *what() const throw()
        {
            return "Attempted to create invalid separation unit. \
                    Concentration and tails pipes must feed into \
					different outputs. No self-recycling allowed.";
        }
    };

    // the main public interface
    // setters
    void inc_colour() { colour_code_++; }

    // getters
    int tails() { return tails_num_; };
    int conc() { return conc_num_; };
    int colour() { return colour_code_; };
    int id() { return id_; }

    // stores the 'parent' nodes for this particular unit, i.e.
    // the other units whose T or C output connects to this particular unit.
    std::vector<int> parents{}; // initialise as blank vector

private:
    // index of the unit to which this unit’s concentrate stream is connected
    int conc_num_{-1};
    // index of the unit to which this unit’s concentrate stream is connected
    int tails_num_{-1};
    // represents whether or not the node has been visited.
    // number gets incremented every time the same set of nodes is traversed.
    int colour_code_{0};
    // stores unique ID assigned to each unit at creation.
    int id_{-1};
};

#endif // !_CUNIT_
