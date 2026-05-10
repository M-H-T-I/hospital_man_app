#include <cstdio>
#include <cstdlib>

#include "FileHandler.hpp"
#include "UI.hpp"



int main()
{

    AppState app;
    app.loadAll();

    ui_run(app);  

    return 0;
}