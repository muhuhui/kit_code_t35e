#if !defined(_LIFTARMCOMMAND_H)
#define _LIFTARMCOMMAND_H

#include <Command.h>

class LiftArmCommand:public Command
{
public:
    LiftArmCommand(){ name = "LiftArmCommand";}
private:
    int exec();
};
#endif

