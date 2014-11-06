#include "uilayout.h"

namespace ui {

layout::layout(const Rect& size, layout_type type, widget *parent)
	: widget(parent),
	  m_layout_type(type)
{
	setSize(size);
}

layout::~layout(void)
{
}

void layout::addChild(widget *child)
{
	if (child != NULL) {
		Rect tmp;

		m_children.push_back(child);
		m_layout.push_back(tmp);

		updateLayouts();
	}
}

void layout::drawChildren(canvas* console)
{
	for (int i = 0; i < static_cast<int>(m_children.size()); i++) {
		if (m_children[i]->isVisible()) {
			m_children[i]->setSize(m_layout[i]);
			m_children[i]->draw(console);
		}
	}
}

void layout::draw(canvas* console)
{
	if (isVisible()) {
		drawChildren(console);
	}
}

void layout::setSize(const Rect &size)
{
	m_size = size;

	if (m_children.size() > 0) {
		updateLayouts();
	}
}

void layout::redrawNotify()
{
	widget::redrawNotify();

	updateLayouts();
}

void layout::updateLayouts()
{
	int height = m_size.height();
	int width = m_size.width();
	int nchildren = static_cast<int>(m_children.size());
	int i;

	bool ish = (m_layout_type == LAYOUT_HORIZONTAL);

	std::vector<int> sizes(m_layout.size(), 0);

	/* layouts are sized based on their policy in the following order:
	 *  POLICY_FIXED, POLICY_MINIMUM, POLICY_MAXIMUM, POLICY_IGNORED
	 * in that order.  Widgets with the same policy are sized in numerical
	 * order.
	 */

	// size for our layout type first
	// FIXED
	for (i = 0; i < static_cast<int>(m_children.size()); i++) {
		if (m_children[i]->isVisible()) {
			if (m_children[i]->getPolicy().getRule(ish) == policy::POLICY_FIXED) {
				// record the size here
				sizes[i] = m_children[i]->getPolicy().getSize(ish);
				// update the overall container size available
				if (ish) { width -= sizes[i]; }
				else { height -= sizes[i]; }
				nchildren--;
			}
		} else {
			nchildren--;
		}
	}

	int hn = (nchildren == 0 ? 0 : height / nchildren);
	int wn = (nchildren == 0 ? 0 : width / nchildren);

	// I will need to do these policies separately I think
	for (i = 0; i < static_cast<int>(m_children.size()); i++) {
		if (m_children[i]->isVisible() == false) continue;

		switch (m_children[i]->getPolicy().getRule(ish)) {
		case policy::POLICY_MINIMUM:
			sizes[i] = m_children[i]->getPolicy().getSize(ish);
			
			if (ish) {
				// check if our min size is less than our stretch size
				if (sizes[i] >= wn) {
					// nope, set our stretch size
					width -= sizes[i];
					nchildren--;

					wn = (nchildren == 0 ? 0 : width / nchildren);
				} else {
					sizes[i] = wn;
				}
			} else {
				// check if our min size is less than our stretch size
				if (sizes[i] >= hn) {
					// nope, set our stretch size
					height -= sizes[i];
					nchildren--;

					hn = (nchildren == 0 ? 0 : height / nchildren);
				} else {
					sizes[i] = hn;
				}
			}
			break;
		case policy::POLICY_MAXIMUM:
			sizes[i] = m_children[i]->getPolicy().getSize(ish);
			
			if (ish) {
				// check if our max size is greater than our stretch size
				if (sizes[i] < wn) {
					// nope, set our stretch size
					width -= sizes[i];
					nchildren--;

					wn = (nchildren == 0 ? 0 : width / nchildren);
				} else {
					sizes[i] = wn;
				}
			} else {
				// check if our max size is greater than our stretch size
				if (sizes[i] < hn) {
					// nope, set our stretch size
					height -= sizes[i];
					nchildren--;

					hn = (nchildren == 0 ? 0 : height / nchildren);
				} else {
					sizes[i] = hn;
				}
			}
			break;
		case policy::POLICY_IGNORED:
			if (ish) {
				sizes[i] = wn;
			} else {
				sizes[i] = hn;
			}
			break;
		default: break;
		}
	}

	int sum = 0;

	switch (m_layout_type)
	{
	case LAYOUT_VERTICAL:
		for (i = 0; i < static_cast<int>(m_layout.size()); i++) {
			m_layout[i] = Rect(m_size.top() + sum, m_size.left(),
							   m_layout[i].top() + sizes[i], m_size.right());

			sum += sizes[i];
		}
		break;
	case LAYOUT_HORIZONTAL:
		for (i = 0; i < static_cast<int>(m_layout.size()); i++) {
			m_layout[i] = Rect(m_size.top(), m_size.left() + sum,
							   m_size.bottom(), m_layout[i].left() + sizes[i]);

			sum += sizes[i];
		}
		break;
	}
}

}
