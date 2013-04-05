/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012 Icinga Development Team (http://www.icinga.org/)        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "remoting/i2-remoting.h"
#include "remoting/requestmessage.h"
#include "remoting/responsemessage.h"
#include "base/dynamicobject.h"
#include "base/stream.h"
#include <boost/signals2.hpp>

namespace icinga
{

class EndpointManager;

/**
 * An endpoint that can be used to send and receive messages.
 *
 * @ingroup remoting
 */
class I2_REMOTING_API Endpoint : public DynamicObject
{
public:
	typedef shared_ptr<Endpoint> Ptr;
	typedef weak_ptr<Endpoint> WeakPtr;

	typedef void (Callback)(const Endpoint::Ptr&, const Endpoint::Ptr&, const RequestMessage&);

	explicit Endpoint(const Dictionary::Ptr& serializedUpdate);
	~Endpoint(void);

	static Endpoint::Ptr GetByName(const String& name);

	Stream::Ptr GetClient(void) const;
	void SetClient(const Stream::Ptr& client);

	void RegisterSubscription(const String& topic);
	void UnregisterSubscription(const String& topic);
	bool HasSubscription(const String& topic) const;

	Dictionary::Ptr GetSubscriptions(void) const;
	void SetSubscriptions(const Dictionary::Ptr& subscriptions);

	bool IsLocalEndpoint(void) const;
	bool IsConnected(void) const;

	void ProcessRequest(const Endpoint::Ptr& sender, const RequestMessage& message);
	void ProcessResponse(const Endpoint::Ptr& sender, const ResponseMessage& message);

	void ClearSubscriptions(void);

	void RegisterTopicHandler(const String& topic, const boost::function<Callback>& callback);

	String GetNode(void) const;
	String GetService(void) const;

	static Endpoint::Ptr MakeEndpoint(const String& name, bool replicated, bool local = true);

	static boost::signals2::signal<void (const Endpoint::Ptr&)> OnConnected;

private:
	Attribute<bool> m_Local;
	Attribute<Dictionary::Ptr> m_Subscriptions;
	Attribute<String> m_Node;
	Attribute<String> m_Service;

	Stream::Ptr m_Client;

	bool m_ReceivedWelcome; /**< Have we received a welcome message
				     from this endpoint? */
	bool m_SentWelcome; /**< Have we sent a welcome message to this
			         endpoint? */

	std::map<String, shared_ptr<boost::signals2::signal<Callback> > > m_TopicHandlers;

	void MessageThreadProc(const Stream::Ptr& stream);
};

}

#endif /* ENDPOINT_H */
