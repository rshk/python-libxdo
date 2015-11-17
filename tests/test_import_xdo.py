def test_module_can_be_imported():
    import xdo  # noqa


def test_library_version_is_supported():
    import xdo
    assert xdo.Xdo.version_info()[0] == 3
