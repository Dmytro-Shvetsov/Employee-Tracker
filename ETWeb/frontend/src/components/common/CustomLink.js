import React from 'react';
import PropTypes from 'prop-types';
import { withRouter, useHistory } from 'react-router-dom';

const CustomLink = ({ to, onClick, tag: Tag, staticContext, ...rest }) => {
    const history = useHistory();
    return <Tag
        {...rest}
        onClick={event => {
            onClick(event);
            history.push(to);
        }}
    />
};

CustomLink.propTypes = {
    to: PropTypes.string.isRequired,
    children: PropTypes.node.isRequired,
    onClick: PropTypes.func
};

CustomLink.defaultProps = {
    onClick: event => {}
};

export default withRouter(CustomLink);