#ifndef HEMELB_UNITTESTS_REDBLOOD_FADEINOUTINTEGRATION_H
#define HEMELB_UNITTESTS_REDBLOOD_FADEINOUTINTEGRATION_H

#include <cppunit/extensions/HelperMacros.h>
#include <memory>

#include "lb/BuildSystemInterface.h"
#include "Traits.h"
#include "redblood/Mesh.h"
#include "redblood/Cell.h"
#include "redblood/CellController.h"
#include "unittests/helpers/FolderTestFixture.h"

namespace hemelb
{
  namespace unittests
  {
    namespace redblood
    {
      class FadeInOutIntegrationTests : public hemelb::unittests::helpers::FolderTestFixture
      {
        CPPUNIT_TEST_SUITE(FadeInOutIntegrationTests);
          CPPUNIT_TEST(testIntegration);CPPUNIT_TEST_SUITE_END()
          ;

          typedef Traits<>::ChangeKernel<lb::GuoForcingLBGK>::Type Traits;
          typedef hemelb::redblood::CellController<Traits::Kernel> CellControl;
          typedef SimulationMaster<Traits> MasterSim;

        public:
          void setUp()
          {
            FolderTestFixture::setUp();
            CopyResourceToTempdir("large_cylinder.xml");
            CopyResourceToTempdir("large_cylinder.gmy");
            CopyResourceToTempdir("red_blood_cell.txt");

            std::vector<std::string> intel;
            intel.push_back("simulation");
            intel.push_back("steps");
            intel.push_back("value");
            ModifyXMLInput("large_cylinder.xml", std::move(intel), 100);
            intel.clear();
            intel.push_back("redbloodcells");
            intel.push_back("insertcondition");
            intel.push_back("iterations");
            intel.push_back("value");
            ModifyXMLInput("large_cylinder.xml", std::move(intel), 10);

            argv[0] = "hemelb";
            argv[1] = "-in";
            argv[2] = "large_cylinder.xml";
            argv[3] = "-i";
            argv[4] = "1";
            argv[5] = "-ss";
            argv[6] = "1111";
            options = std::make_shared<hemelb::configuration::CommandLine>(argc, argv);

            master = std::make_shared<MasterSim>(*options, Comms());
          }

          void tearDown()
          {
            master->Finalise();
            master.reset();
          }

          void testIntegration()
          {
            // add callback to put cell positions in a vector
            std::vector<std::vector<LatticePosition>> positions;
            std::function<void(const hemelb::redblood::CellContainer &)> output_callback =
                [&positions](const hemelb::redblood::CellContainer & cells)
                {
                  std::vector<LatticePosition> iteration;
                  for (auto cell: cells)
                  iteration.push_back(cell->GetBarycenter());
                  positions.push_back(iteration);
                };
            controller->AddCellChangeListener(output_callback);

            // run the simulation
            master->RunSimulation();

            // check that we have the cell positions
            CPPUNIT_ASSERT_EQUAL(100ul, positions.size());
          }

        private:
          std::shared_ptr<MasterSim> master;
          std::shared_ptr<hemelb::configuration::CommandLine> options;
          int const argc = 7;
          char const * argv[7];

      };
      // class FadeInOutIntegrationTests

      CPPUNIT_TEST_SUITE_REGISTRATION(FadeInOutIntegrationTests);

    } // namespace redblood
  } // namespace unittests
} // namespace hemelb

#endif