#include "conduit/assets/assetsManager.h"
#include "conduit/assets/mesh.h"
#include "conduit/assets/assetInfo.h"
#include "conduit/assets/shader.h"
#include "conduit/assets/texture.h"
#include "conduit/defines.h"

#include "conduit/application.h"
#include "conduit/internal/assets/assetLoaderFuns.h"
#include "conduit/internal/assets/assetParser.h"
#include "conduit/internal/assets/assetParserFuns.h"
#include "conduit/internal/assets/assetsCache.h"
#include "conduit/logging.h"

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    std::string appName() const override { return "Sanbox app"; }

    void startup() override {
        AssetsManager manager;

        {
            Handle<Shader> frag = manager.get<Shader>("builtinFrag");
            Handle<Shader> frag2 = manager.get<Shader>("builtinFrag");
        }

        Handle<Shader> frag = manager.get<Shader>("builtinFrag");
    }
    
    void update(f64) override {

    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
