#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <json-c/json.h>
#include <systemd/sd-event.h>
extern "C"
{
	#define AFB_BINDING_VERSION 2
	#include <afb/afb-binding.h>
};

struct TimedEvent {
	int interval;
	afb_event event;
	std::string name;
	std::string eventName;
};

struct Property {
	/**
	 * alternatively, instead of a value per type, we could use boost::any, or in c++17 variant.
	 */
	std::string type;
	std::string description;
	std::string lowMessageName;
	int interval;
	int value_int;
	bool value_bool;
	double value_double;
	std::string value_string;

	Property() { interval = 0; value_int = 0; value_bool = false; value_double = 0.0;}
};

class High
{
public:
	High();
	void treatMessage(json_object *message);
	bool subscribe(afb_req request);
	bool unsubscribe(afb_req request);
	bool get(afb_req request, json_object **json);
	void tick(sd_event_source *source, const long &now, void *interv);
	void startTimer(const int &t);
	~High();
	void parseConfigAndSubscribe(const std::string& confd);
	static bool startsWith(const std::string &s, const std::string &val);
	static void callBackFromSubscribe(void *handle, int iserror, json_object *result);
private:
	std::map<std::string, afb_event> events;
	std::map<int, std::vector<TimedEvent>> timedEvents;
	std::map<std::string, std::map<std::string, Property>> registeredObjects;
	std::map<std::string, std::set<std::string>> lowMessagesToObjects;
	std::set<int> timers;
	std::string generateId() const;
	json_object *generateJson(const std::string &messageObject, std::vector<std::string> *fields = nullptr);
	void registerObjects(const std::string& uri, const std::map<std::string, Property>& properties);
	std::map<std::string, std::map<std::string, Property>> loadDefinitions(json_object* definitionsJ) const;
	void loadResources(json_object* resourcesJ);
};