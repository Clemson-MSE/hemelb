#ifndef HEMELB_REDBLOOD_RBC_INSERTER_H
#define HEMELB_REDBLOOD_RBC_INSERTER_H

#include <iostream>
#include <memory>
#include <functional>
#include "io/xml/XmlAbstractionLayer.h"
#include "lb/iolets/InOutLet.h"
#include "Mesh.h"
#include "Cell.h"
#include "units.h"
#include "util/Matrix3D.h"

namespace hemelb
{
  namespace redblood
  {

    /**
     * The RBC Inserter inserts cells into the simulation while some condition
     * evaluates to true.  The cell is inserted into an inlet or the start of a
     * flow extension (if the inlet has a flow extension defined).
     * The shape of the cell is read from a text file.
     */
    class RBCInserter
    {
      public:
        /**
         * Creates an RBC Inserter.
         *
         * @param condition a cell will only be inserted on a LB step if this
         * condition evaluates to true
         * @param cell to insert. Inserted as is. E.g. same position etc.
         * @param scale the scale of the cell to insert
         */
        RBCInserter(std::function<bool()> condition, std::unique_ptr<CellBase const> cell) :
            condition(condition), cell(std::move(cell)), barycenter(this->cell->GetBarycenter())
        {
        }
        RBCInserter(RBCInserter &&c) :
            condition(std::move(c.condition)), cell(std::move(c.cell)),
            barycenter(std::move(c.barycenter))
        {
        }
        RBCInserter(RBCInserter const&c) :
            condition(c.condition), cell(c.cell->clone()), barycenter(c.barycenter)
        {
        }
        void operator=(RBCInserter const &c)
        {
          condition = c.condition;
          cell = c.cell->clone();
        }
        void operator=(RBCInserter &&c)
        {
          condition = std::move(c.condition);
          cell = std::move(c.cell);
        }

        /**
         * Cell insertion callback called on each step of the simulation.  Cells
         * are inserted into the inlets while condition() evaluates to true.
         * Multiple cells are potentially inserted on each iteration.
         *
         * @param insertFn the function to insert a new cell into the simulation
         *
         * @see hemelb::redblood::CellArmy::SetCellInsertion
         * @see hemelb::redblood::CellArmy::CallCellInsertion
         */
        void operator()(CellInserter insertFn) const
        {
          if (condition())
          {
            log::Logger::Log<log::Debug, log::OnePerCore>(
                "Dropping one cell at (%f, %f, %f)", barycenter.x, barycenter.y, barycenter.z);
            insertFn(drop());
          }
        }

        virtual CellContainer::value_type drop() const
        {
          return CellContainer::value_type(cell->clone().release());
        }

      private:
        //! When to insert cells
        std::function<bool()> condition;
        //! The shape of the cells to insert
        std::unique_ptr<CellBase const> cell;
        //! barycenter -- for logging
        LatticePosition barycenter;
    };

    //! Red blood cell inserter that adds random rotation and translation to each cell
    class RBCInserterWithPerturbation : RBCInserter {
      public:
        RBCInserterWithPerturbation(
            std::function<bool()> condition, std::unique_ptr<CellBase const> cell,
            util::Matrix3D const &initialRotation, Angle dtheta, Angle dphi,
            LatticePosition const& dx, LatticePosition const& dy)
          : RBCInserter(condition, std::move(cell)), initialRotation(initialRotation),
            dtheta(dtheta), dphi(dphi), dx(dx), dy(dy)
        {
        }

        //! Rotates and translates the template cell according to random dist
        CellContainer::value_type drop() const override;

        using RBCInserter::operator();
      private:
        //! Rotation to flow axis + offset
        util::Matrix3D initialRotation;
        //! Range of values over which to pick extra rotation.
        Angle dtheta, dphi;
        //! Two vectors alongst which to move cell
        LatticePosition dx, dy;
    };
  }
}

#endif