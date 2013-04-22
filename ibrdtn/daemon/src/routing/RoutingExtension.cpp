/*
 * RoutingExtension.cpp
 *
 * Copyright (C) 2013 IBR, TU Braunschweig
 *
 * Written-by: Johannes Morgenroth <morgenroth@ibr.cs.tu-bs.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "routing/RoutingExtension.h"
#include "routing/BaseRouter.h"
#include "core/BundleCore.h"
#include <ibrcommon/Logger.h>

namespace dtn
{
	namespace routing
	{
		BaseRouter *RoutingExtension::_router = NULL;
		const std::string RoutingExtension::TAG = "RoutingExtension";

		/**
		 * base implementation of the Extension class
		 */
		RoutingExtension::RoutingExtension()
		{ }

		RoutingExtension::~RoutingExtension()
		{ }

		BaseRouter& RoutingExtension::operator*()
		{
			return *_router;
		}

		/**
		 * Transfer one bundle to another node.
		 * @param destination The EID of the other node.
		 * @param id The ID of the bundle to transfer. This bundle must be stored in the storage.
		 */
		void RoutingExtension::transferTo(const dtn::data::EID &destination, const dtn::data::BundleID &id)
		{
			// lock the list of neighbors
			ibrcommon::MutexLock l(_router->getNeighborDB());

			// get the neighbor entry for the next hop
			NeighborDatabase::NeighborEntry &entry = _router->getNeighborDB().get(destination);

			// transfer bundle to the neighbor
			transferTo(entry, id);
		}

		void RoutingExtension::transferTo(NeighborDatabase::NeighborEntry &entry, const dtn::data::BundleID &id)
		{
			// acquire the transfer of this bundle, could throw already in transit or no resource left exception
			entry.acquireTransfer(id);

			try {
				// transfer the bundle to the next hop
				dtn::core::BundleCore::getInstance().transferTo(entry.eid, id);

				IBRCOMMON_LOGGER_DEBUG_TAG(RoutingExtension::TAG, 20) << "bundle [" << id.toString() << "] queued for delivery to " << entry.eid.getString() << IBRCOMMON_LOGGER_ENDL;
			} catch (const dtn::core::P2PDialupException&) {
				// release the transfer
				entry.releaseTransfer(id);

				// and abort the query
				throw NeighborDatabase::NeighborNotAvailableException();
			}
		}

		bool RoutingExtension::isRouting(const dtn::data::EID &eid)
		{
			if (eid.getApplication() == "routing")
			{
				return true;
			}
			else if ((eid.getScheme() == dtn::data::EID::CBHE_SCHEME) && (eid.getApplication() == "50"))
			{
				return true;
			}

			return false;
		}

	} /* namespace routing */
} /* namespace dtn */