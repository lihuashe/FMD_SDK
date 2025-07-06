
# 版本管理
# 输出最近的有tag的commit
TAG_COMMIT := $(shell git rev-list --abbrev-commit --tags --max-count=1)
# 获取该commit的TAG
ifeq ($(OS), Windows_NT)
TAG := $(shell git describe --abbrev=0 --tags ${TAG_COMMIT} 2> nul || true)
else
TAG := $(shell git describe --abbrev=0 --tags ${TAG_COMMIT} 2>/dev/null || true)
endif
# 获取最新的commit
COMMIT := $(shell git rev-parse --short HEAD)
# 获取tag_commit的日期
DATE := $(shell git log -1 --tags $(TAG_COMMIT) --format=%cd --date=format:"%Y%m%d")
# 移除'v'
VERSION := $(TAG:v%=%)

# 主版本号
VERSION_MAJOR := $(word 1,$(subst ., ,$(VERSION)))
# 此版本号
VERSION_MINOR := $(word 2,$(subst ., ,$(VERSION)))
# build版本: finds the most recent tag that is reachable from a commit.
VERSION_REVISION := $(word 2,$(subst -, ,$(shell git describe)))
# commit id
VERSION_GCID := $(COMMIT)
# 时间
BUILDING_TIME := $(DATE)


# 如果获取的VERSION为空,使用默认的项目名称
ifeq ($(VERSION),)
    VERSION := unknown.$(VERSION_REVISION)_$(COMMIT)-$(DATE)
else
    # 如果最新的commit和tag的comit不相符,表示输出的版本不是最新的,用特殊格式输出:version.[revision].[commit]
    ifneq ($(COMMIT), $(TAG_COMMIT))
        VERSION := $(VERSION).$(VERSION_REVISION)_$(COMMIT)
    endif
    # 如果当前存在未提交的修改,版本带上dirty
    ifneq ($(shell git status --porcelain),)
        VERSION := $(VERSION)_dirty
    endif

endif

CFLAGS_VERSION=-DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_REVISION=$(VERSION_REVISION) \
    -DVERSION_GCID=$(VERSION_GCID) -DBUILDING_TIME=$(BUILDING_TIME)
