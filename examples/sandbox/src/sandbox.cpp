#include "conduit/assets/assetInfo.h"
#include "conduit/assets/handle.h"
#include "conduit/defines.h"

#include "conduit/application.h"
#include "conduit/internal/assets/assetLocator.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    std::string appName() const override { return "Sanbox app"; }

    void startup() override {
        internal::AssetLocator loc;
    }
    
    void update(f64) override {

    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
