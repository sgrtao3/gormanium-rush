#include "CUnit.h"

SeparationUnit::SeparationUnit(int conc, int tails, int id, int parent) : conc_num_{conc},
                                                                          tails_num_{tails},
                                                                          id_{id}
{
    // throw error if self-recycle or both feed to same output
    if (id == conc || id == tails || conc == tails)
    {
        throw BadSeparationUnit();
    }
}
