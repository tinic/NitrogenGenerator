#include "mcp.h"

#include "Core/Inc/main.h"

MCP &MCP::instance() {
    static MCP mcp;
    if (!mcp.initialized) {
        mcp.initialized = true;
        mcp.init();
    }
    return mcp;
}

void MCP::Slice() {
}
