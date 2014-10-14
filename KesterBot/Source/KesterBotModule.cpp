/* 
 +----------------------------------------------------------------------+
 | KesterBot                                                            |
 +----------------------------------------------------------------------+
 | National University of Singapore                                     |
 +----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------+
 | Author: Steven Kester Yuwono <a0080415@nus.edu.sg>                   |
 +----------------------------------------------------------------------+
*/

#include "Common.h"
#include "KesterBotModule.h"


BWAPI::AIModule * __NewAIModule()
{
	return new KesterBotModule();
}

KesterBotModule::KesterBotModule()  {}
KesterBotModule::~KesterBotModule() {}

void KesterBotModule::onStart()
{
	//this will be the speed of the game, normal game speed = 30, tournament SSCAI = 20, fastest = 0;
	BWAPI::Broodwar->setLocalSpeed(25);

	BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
	//BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);

	if (Options::Modules::USING_GAMECOMMANDER)
	{
		BWTA::readMap();
		BWTA::analyze();
	}
	
	if (Options::Modules::USING_MICRO_SEARCH)
	{
		Search::StarcraftData::init();
		MicroSearch::Hash::initHash();
		micro.onStart();
	}
}

void KesterBotModule::onEnd(bool isWinner) 
{
	if (Options::Modules::USING_GAMECOMMANDER)
	{
		StrategyManager::Instance().onEnd(isWinner);

		std::stringstream result;
		std::string win = isWinner ? "win" : "lose";

		result << "Game against " << BWAPI::Broodwar->enemy()->getName() << " " << win << " with strategy " << StrategyManager::Instance().getCurrentStrategy() << "\n";

		Logger::Instance().log(result.str());
	}
}

void KesterBotModule::onFrame()
{
	if (Options::Modules::USING_GAMECOMMANDER) 
	{ 
		gameCommander.update(); 
	}
	
	if (Options::Modules::USING_ENHANCED_INTERFACE)
	{
		eui.update();
	}

	if (Options::Modules::USING_MICRO_SEARCH)
	{
		micro.update();
	}

	if (Options::Modules::USING_REPLAY_VISUALIZER)
	{
		BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->getAllUnits())
		{
			BWAPI::Broodwar->drawTextMap(unit->getPosition().x(), unit->getPosition().y(), "   %d", unit->getPlayer()->getID());

			if (unit->isSelected())
			{
				BWAPI::Broodwar->drawCircleMap(unit->getPosition().x(), unit->getPosition().y(), 1000, BWAPI::Colors::Red);
			}
		}
	}

	//Visualizer::Instance().setBWAPIState();
	//Visualizer::Instance().onFrame();
}

void KesterBotModule::onUnitDestroy(BWAPI::Unit * unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
	if (Options::Modules::USING_ENHANCED_INTERFACE) { eui.onUnitDestroy(unit); }
}

void KesterBotModule::onUnitMorph(BWAPI::Unit * unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
}

void KesterBotModule::onSendText(std::string text) 
{ 
	BWAPI::Broodwar->sendText(text.c_str());


	if (Options::Modules::USING_REPLAY_VISUALIZER && (text.compare("sim") == 0))
	{
		BWAPI::Unit * selected = NULL;
		BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->getAllUnits())
		{
			if (unit->isSelected())
			{
				selected = unit;
				break;
			}
		}

		if (selected)
		{
			#ifdef USING_VISUALIZATION_LIBRARIES
				ReplayVisualizer rv;
				rv.launchSimulation(selected->getPosition(), 1000);
			#endif
		}
	}
	else if (text.compare("sim") != 0)
	{
		BWAPI::Broodwar->setLocalSpeed(atoi(text.c_str()));
	}
}

void KesterBotModule::onUnitCreate(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitCreate(unit); }
}

void KesterBotModule::onUnitShow(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitShow(unit); }
}

void KesterBotModule::onUnitHide(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitHide(unit); }
}

void KesterBotModule::onUnitRenegade(BWAPI::Unit * unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitRenegade(unit); }
}
