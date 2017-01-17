// Copyright (c) 2017 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_ATOM_API_BROWSER_VIEW_H_
#define ATOM_BROWSER_API_ATOM_API_BROWSER_VIEW_H_

#include <memory>
#include <string>

#include "atom/browser/api/trackable_object.h"
#include "native_mate/handle.h"

namespace mate {
class Arguments;
class Dictionary;
}

namespace atom {

namespace api {

class WebContents;
class Window;

class BrowserView : public mate::TrackableObject<BrowserView> {
 public:
  static mate::WrappableBase* New(mate::Arguments* args);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  api::Window* window() const { return api_window_; }
  api::WebContents* web_contents() const { return api_web_contents_; }
  int32_t ID() const;

 protected:
  BrowserView(v8::Isolate* isolate,
              v8::Local<v8::Object> wrapper,
              const mate::Dictionary& options);
  ~BrowserView() override;

 private:
  void Init(v8::Isolate* isolate,
            v8::Local<v8::Object> wrapper,
            const mate::Dictionary& options,
            mate::Handle<class WebContents> web_contents);

  v8::Local<v8::Value> WebContents(v8::Isolate* isolate);

  v8::Global<v8::Value> web_contents_;
  v8::Global<v8::Value> window_;

  api::WebContents* api_web_contents_;
  api::Window* api_window_;

  DISALLOW_COPY_AND_ASSIGN(BrowserView);
};

}  // namespace api

}  // namespace atom

#endif  // ATOM_BROWSER_API_ATOM_API_BROWSER_VIEW_H_
