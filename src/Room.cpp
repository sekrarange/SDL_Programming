// Copyright anssi.grohn@karelia.fi (c) 2014.
// Licensed under GPLv3.
#include "Room.h"
#include "Game.h"
#include <cstdlib>
#include <iostream>
#include <tinyxml2.h>
#include <sstream>
#include <Logger.h>
#include <GameScene.h>
#define CurrentPage *(GameScene::GetCurrentPage())
////////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace tinyxml2;
#define g_Game (*Game::GetInstance())
////////////////////////////////////////////////////////////////////////////////
Room::Room()
{
  m_aRooms[0] = NULL;
  m_aRooms[1] = NULL;
  m_aRooms[2] = NULL;
  m_aRooms[3] = NULL;
}
////////////////////////////////////////////////////////////////////////////////
Room::~Room()
{
  
}
////////////////////////////////////////////////////////////////////////////////
void
Room::SetNextRoom( Direction d, Room * pRoom )
{
  m_aRooms[d] = pRoom;
}
////////////////////////////////////////////////////////////////////////////////
Room * 
Room::GetNextRoom( Direction d )
{
  return m_aRooms[d];
}
////////////////////////////////////////////////////////////////////////////////
void 
Room::SetGame( Game * pGame )
{
  m_pGame = pGame;
}
////////////////////////////////////////////////////////////////////////////////
Game * 
Room::GetGame() const
{
  return m_pGame;
}
////////////////////////////////////////////////////////////////////////////////
void 
Room::Load( XMLDocument & doc, const string & id)
{
  // get first element
  XMLElement *pElem = doc.FirstChildElement( "Room");
  // if name is specified
  if ( id != "" )
  {
    // retrieve element until nothing is found or attribute(id) matches to param
    while( pElem != NULL && pElem->Attribute("id") != id )
    {
      pElem = pElem->NextSiblingElement("Room");
    }
  }
  // brace for impact
  if ( pElem == NULL ) 
  {
    ostringstream ss;
    ss << "Could not find element with id " << id << "\n";
    throw ElementMissingException(ss.str());
  }
  
  // set properties and items
  Load(*pElem);
  
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Load(tinyxml2::XMLElement & elem)
{
	GameObject::Load(elem);
	const char * attrib = elem.Attribute("class");
	if (attrib) SetClassHint(attrib);

	XMLElement *overview = elem.FirstChildElement("Overview");
	if (overview)
	{
		const char *text = overview->GetText();
		if (text) SetEnterMessage(text);
		else g_Log << "Overview defined, but empty\n";
	}
	else g_Log << "No Overview message defined...";
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Save( XMLPrinter & p )
{
  p.OpenElement("Room");
  p.PushAttribute("name", GetName().c_str());
  p.PushAttribute("id", GetId().c_str());
  p.PushAttribute("description", GetDescription().c_str());
  if ( !m_classHint.empty())   p.PushAttribute("class", m_classHint.c_str());
  p.PushAttribute("match", GetNamePattern().str().c_str());

  SaveProperties(p);

  p.OpenElement("Items");
  for(auto i : GetItems() )
  {
    i->Save(p);
  }
  p.CloseElement();
  p.OpenElement("Overview");
  p.PushText(GetEnterMessage().c_str());
  p.CloseElement();
  p.CloseElement();
}
////////////////////////////////////////////////////////////////////////////////
void
Room::SetEnterMessage( const std::string & msg )
{
  m_strEnterMessage = msg;
}
////////////////////////////////////////////////////////////////////////////////
const std::string & 
Room::GetEnterMessage() const
{
  return m_strEnterMessage;
}
////////////////////////////////////////////////////////////////////////////////
void 
Room::Execute(QuitCommand & cmd)
{

}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute(MoveCommand & cmd)
{
  if ( cmd.m_pTo == this )
  {
    if ( HasProperty("deadly") && GetProperty("deadly").As<bool>() )
    {
      // Report death message attached to room
      CurrentPage << GetProperty("deathmessage").As<string>() <<  "\n";
	  g_Game.SetProperty("running", false);
    }
    else 
    {
		CurrentPage << "\n" << GetName() << "\n";
		CurrentPage << GetEnterMessage() << "\n";


      list<string> items;
      for(auto item : GetItems()) 
      {
	if ( !item->HasProperty("hidden") || item->GetProperty("hidden").As<bool>() == false )
	  items.push_back(item->GetName());
      }
    
      if ( items.empty()) 
      {
		  CurrentPage << "There seems to be nothing in the room.";
      }
      else 
      {
		  CurrentPage << "You can see " << Game::MakeReadable(items) << " in here.";
      }
    }
  }
  else 
  {
	  CurrentPage << "You leave " << GetName() << ".";
    if ( cmd.m_Dir == South && HasProperty("south_exit_message") )
		CurrentPage << GetProperty("south_exit_message").As<string>() << ".";
    if ( cmd.m_Dir == West && HasProperty("west_exit_message") )
		CurrentPage << GetProperty("west_exit_message").As<string>() << ".";
    if ( cmd.m_Dir == North && HasProperty("north_exit_message") )
		CurrentPage << GetProperty("north_exit_message").As<string>() << ".";
    if ( cmd.m_Dir == East && HasProperty("east_exit_message") )
		CurrentPage << GetProperty("east_exit_message").As<string>() << ".";
  }
  CurrentPage << "\n";
}


////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( UnknownCommand & cmd)
{

}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( TakeCommand & cmd )
{
  Inventory & items = GetItems();
  GameObject * item = items[cmd.m_strWhat];
  if ( item ) 
  {
    items.remove(item);
    g_Game.GetPlayer().GetItems().push_back(item);
	CurrentPage << "You picked up " << item->GetName() << "\n";

    // Quick hack for picking up torch, staircase becomes safe.
    if ( item->GetId() == "ID_TORCH" )
    {
      g_Game.GetRooms()["RID_STAIRS_DARKNESS"]->SetProperty("deadly",false);
    }
    
  } 
  else if ( cmd.m_strWhat.empty())
  {
	  CurrentPage << "Can you be more specific?\n";
  }
  else 
  {
	  CurrentPage << "There is no such item.\n";
  }
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( DropCommand & cmd)
{

  GameObject * item = g_Game.GetPlayer().GetItems()[cmd.m_strWhat];
  if ( item )
  {
    g_Game.GetPlayer().GetItems().remove(item);
    g_Game.GetCurrentRoom()->GetItems().push_back(item); 
    // Quick hack for dropping torch, staircase becomes deadly again
    if ( item->GetId() == "ID_TORCH" )
    {
      g_Game.GetRooms()["RID_STAIRS_DARKNESS"]->SetProperty("deadly", true);
    }
	CurrentPage << "You dropped " << item->GetName() << "\n";
  }
  else 
  {
	  CurrentPage << "You do not have such item.\n";
  }
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( InventoryCommand & cmd)
{
  
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( LookCommand & cmd )
{
	CurrentPage << "\n" << GetName() << "\n";
	CurrentPage << "\n" << GetEnterMessage() << "\n";
  
	CurrentPage << "From here you can go ";
  list<string> directions;
    
  if ( GetNextRoom(North)) directions.push_back("north");
  if ( GetNextRoom(East))  directions.push_back("east");
  if ( GetNextRoom(South)) directions.push_back("south");
  if ( GetNextRoom(West))  directions.push_back("west");

  CurrentPage << Game::MakeReadable(directions) << ".\n";
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( ExamineCommand & cmd )
{

  Room & room  = *this;
  if ( !cmd.HasTarget() )
  { 
	  CurrentPage << "Examine what? ";
  }
  else if ( cmd.m_strTarget == "room" )
  {
	  CurrentPage << room.GetDescription() << "\n";
    list<string> items;
    for(auto item : room.GetItems()) 
    {
      items.push_back(item->GetName());
    }
    if ( items.empty()) 
    {
		CurrentPage << "There seems to be nothing in the room. ";
    }
    else 
    {
		CurrentPage << "You can see " << Game::MakeReadable(items) << " in here.";
    }
  } 
  else  
  {
    // examining room items.
    GameObject * item = GetItems()[cmd.m_strTarget];
	if (item) CurrentPage << "It appears to be " << item->GetDescription() << ". ";
	else     CurrentPage << "I do not know how to examine " << cmd.m_strTarget << ". ";
  }
  CurrentPage << "\n";
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( UseCommand & cmd )
{
	CurrentPage << "You are not making any sense.\n";
}
////////////////////////////////////////////////////////////////////////////////
void
Room::Execute( NullCommand & cmd )
{
  
}
////////////////////////////////////////////////////////////////////////////////
void
Room::SetClassHint( const string & hint )
{
  m_classHint = hint;
}
////////////////////////////////////////////////////////////////////////////////
