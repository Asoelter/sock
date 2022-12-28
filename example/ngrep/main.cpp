#include "nylon/libngrep/ngrep.h"
#include "nylon/nylon_test_messages.h"

int main(int argc, char** argv)
{
    auto ngrep = nylon::Ngrep<nylon::TestMessageDefiner>();

    ngrep.init(argc, argv);

    ngrep.run();

    return 0;
}
