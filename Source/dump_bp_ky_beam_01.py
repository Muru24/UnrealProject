import unreal
bp_asset = unreal.EditorAssetLibrary.load_asset('/Game/StylizedToonBeam01/Blueprints/BP_ky_beam_01')
subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
handles = subsystem.k2_gather_subobject_data_for_blueprint(bp_asset)
seen = set()
for h in handles:
    data = unreal.SubobjectDataBlueprintFunctionLibrary.get_data(h)
    obj = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
    if not obj:
        continue
    key = (obj.get_name(), obj.get_class().get_name())
    if key in seen:
        continue
    seen.add(key)
    if obj.get_class().get_name() == 'NiagaraComponent':
        print(f'NIAGARA_BEGIN|{obj.get_name()}')
        for prop in ['asset','auto_activate','visible','relative_location','relative_rotation','relative_scale3d','can_ever_affect_navigation','use_attach_parent_bound']:
            try:
                val = obj.get_editor_property(prop)
                if hasattr(val, 'get_path_name'):
                    val = val.get_path_name()
                print(f'NIAGARA_PROP|{obj.get_name()}|{prop}|{val}')
            except Exception as e:
                print(f'NIAGARA_PROP_ERR|{obj.get_name()}|{prop}|{e}')
        try:
            overrides = obj.get_editor_property('override_parameters')
            print(f'NIAGARA_OVERRIDES|{obj.get_name()}|{overrides}')
        except Exception as e:
            print(f'NIAGARA_OVERRIDES_ERR|{obj.get_name()}|{e}')
        print(f'NIAGARA_END|{obj.get_name()}')
