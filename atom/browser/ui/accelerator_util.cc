// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/ui/accelerator_util.h"

#include <stdio.h>

#include <string>
#include <vector>

#include "atom/common/keyboard_util.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"

namespace accelerator_util {

bool StringToKeycodeModifiers(const std::string& shortcut,
                              ui::KeyboardCode* key, bool* shift,
                              bool* control, bool* alt, bool* command,
                              bool* altgr) {
  if (!base::IsStringASCII(shortcut)) {
    LOG(ERROR) << "The accelerator string can only contain ASCII characters";
    return false;
  }

  std::vector<std::string> tokens = base::SplitString(
     shortcut, "+", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);

  *key = ui::VKEY_UNKNOWN;
  for (const auto& token : tokens) {
    bool shifted = false;
    ui::KeyboardCode code = atom::KeyboardCodeFromStr(token, &shifted);
    if (shifted)
      *shift = !*shift;
    switch (code) {
      // The token can be a modifier.
      case ui::VKEY_SHIFT:
        *shift = !*shift;
        break;
      case ui::VKEY_CONTROL:
        *control = !*control;
        break;
      case ui::VKEY_MENU:
        *alt = !*alt;
        break;
      case ui::VKEY_COMMAND:
        *command = !*command;
        break;
      case ui::VKEY_ALTGR:
        *altgr = !*altgr;
        break;
      // Or it is a normal key.
      default:
        *key = code;
    }
  }

  if (*key == ui::VKEY_UNKNOWN) {
    LOG(WARNING) << shortcut << " doesn't contain a valid key";
    return false;
  }

  return true;
}

bool StringToAccelerator(const std::string& shortcut,
                         ui::Accelerator* accelerator) {
  bool shift = false;
  bool control = false;
  bool alt = false;
  bool command = false;
  bool altgr = false;
  ui::KeyboardCode key = ui::VKEY_UNKNOWN;
  if (!StringToKeycodeModifiers(shortcut, &key, &shift, &control, &alt, &command, &altgr)) {
    return false;
  }

  // Now, parse it into an accelerator.
  int modifiers = ui::EF_NONE;
  if (shift) {
    modifiers |= ui::EF_SHIFT_DOWN;
  }
  if (control) {
    modifiers |= ui::EF_CONTROL_DOWN;
  }
  if (alt) {
    modifiers |= ui::EF_ALT_DOWN;
  }
  if (command) {
    modifiers |= ui::EF_COMMAND_DOWN;
  }
  if (altgr) {
    modifiers |= ui::EF_ALTGR_DOWN;
  }

  *accelerator = ui::Accelerator(key, modifiers);
  SetPlatformAccelerator(accelerator);
  return true;
}

void GenerateAcceleratorTable(AcceleratorTable* table,
                              atom::AtomMenuModel* model) {
  int count = model->GetItemCount();
  for (int i = 0; i < count; ++i) {
    atom::AtomMenuModel::ItemType type = model->GetTypeAt(i);
    if (type == atom::AtomMenuModel::TYPE_SUBMENU) {
      auto submodel = model->GetSubmenuModelAt(i);
      GenerateAcceleratorTable(table, submodel);
    } else {
      ui::Accelerator accelerator;
      if (model->GetAcceleratorAtWithParams(i, true, &accelerator)) {
        MenuItem item = { i, model };
        (*table)[accelerator] = item;
      }
    }
  }
}

bool TriggerAcceleratorTableCommand(AcceleratorTable* table,
                                    const ui::Accelerator& accelerator) {
  if (base::ContainsKey(*table, accelerator)) {
    const accelerator_util::MenuItem& item = (*table)[accelerator];
    if (item.model->IsEnabledAt(item.position)) {
      item.model->ActivatedAt(item.position);
      return true;
    }
  }
  return false;
}

}  // namespace accelerator_util
