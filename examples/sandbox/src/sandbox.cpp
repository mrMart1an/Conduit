#include "conduit/assets/mesh.h"
#include "conduit/assets/assetInfo.h"
#include "conduit/assets/shader.h"
#include "conduit/assets/texture.h"
#include "conduit/defines.h"

#include "conduit/application.h"
#include "conduit/internal/assets/assetParser.h"
#include "conduit/internal/assets/assetParserFuns.h"
#include "conduit/logging.h"

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    std::string appName() const override { return "Sanbox app"; }

    void startup() override {
        internal::AssetParser<Shader, Texture, Mesh> loc(
            {
                {"shaders" ,internal::parseShader},
                {"textures", internal::parseTexture},
                {"meshes", internal::parseMesh}
            }
        );

        AssetInfo<Shader> info = loc.getInfo<Shader>("builtinFrag");
        log::core::info(
            "{} {}",
            info.pathSpv().string(),
            info.shaderType() == Shader::Type::Vertex
        );
    }
    
    void update(f64) override {

    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
