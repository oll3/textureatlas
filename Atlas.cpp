#include <stdio.h>

#include "Atlas.h"

using namespace Atlas;

Node:: Node(int left, int top, int right, int bottom) {
  static int idcnt = 0;
  mId = idcnt ++;
  mLeft = left;
  mTop = top;
  mRight = right;
  mBottom = bottom;

  mChild[0] = NULL;
  mChild[1] = NULL;
  mLeaf = true;
  mInUse = false;
  mRect = NULL;
}


Node *Node::insert(NodeRect *rect) {
    
  Node *newNode = NULL;

  int w = rect->getWidth();
  int h = rect->getHeight();

  if (!mLeaf) {

    /* This node is not a leaf - try inserting to its child nodes */

    newNode = mChild[0]->insert(rect);
    if (!newNode) {
      newNode = mChild[1]->insert(rect);
    }

  }
  else if (!mInUse) {

    if (w == getWidth() && h == getHeight()) {
      /* The given size fits perfectly */
      mRect = rect;
      newNode = this;
      mInUse = true;
    }
    else {

#if 0
      /* Swap the width and height */
      if (w > getWidth() || h > getHeight()) {
	int tmp = w;
	w = h;
	h = tmp;
      }
#endif
      if (w <= getWidth() && h <= getHeight()) {
	/* Create new child nodes */

	mLeaf = false;
	
	int dw = getWidth() - w;
	int dh = getHeight() - h;

	if (dw > dh) {
	  mChild[0] = new Node(mLeft, mTop, mLeft + w, mBottom);
	  mChild[1] = new Node(mLeft + w, mTop, mRight, mBottom);
	}
	else {
	  mChild[0] = new Node(mLeft, mTop, mRight, mTop + h);
	  mChild[1] = new Node(mLeft, mTop + h, mRight, mBottom);
	}

	newNode = mChild[0]->insert(rect);

      }
    }
  }

  return newNode;
}


void Node::poTraversal(int level, 
		       void(*callback)(int, Node *, void *),
		       void *param) {
    
  if (mRect) {
    if (callback) {
      callback(level, this, param);
    }
  }

  if (mChild[0]) {
    mChild[0]->poTraversal(level + 1, callback, param);
  }
  if (mChild[1]) {
    mChild[1]->poTraversal(level + 1, callback, param);
  }
}

