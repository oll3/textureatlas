#ifndef _ATLAS_H_
#define _ATLAS_H_


namespace Atlas {

  class NodeRect {
  
  public:
    NodeRect(int w, int h) {
      mWidth = w;
      mHeight = h;
    }

    NodeRect() {
      mWidth = 0;
      mHeight = 0;
    }

    void setSize(int w, int h) {
      mWidth = w;
      mHeight = h;
    }

    int getWidth() {
      return mWidth;
    }

    int getHeight() {
      return mHeight;
    }


  private:
    int mWidth, mHeight;
  };


  class Node {

  public:

    Node(int left, int top, int right, int bottom);

    int getWidth() {
      return (mRight - mLeft);
    }

    int getHeight() {
      return (mBottom - mTop);
    }

    int getLeft() {
      return mLeft;
    }

    int getTop() {
      return mTop;
    }

    int getRight() {
      return mRight;
    }

    int getBottom() {
      return mBottom;
    }

    int getId() {
      return mId;
    }

    bool isLeaf() {
      return mLeaf;
    }

    bool isInUse() {
      return mInUse;
    }

    Node *insert(NodeRect *rect);

    void poTraversal(int level, 
		     void(*callback)(int, Node *, void *), 
		     void *param);

    NodeRect *getRect() {
      return mRect;
    }

  private:
  
    int mId;
    int mLeft, mRight, mTop, mBottom;
    NodeRect *mRect;
    bool mLeaf;
    Node *mChild[2];
    bool mInUse;
  };


}

#endif

